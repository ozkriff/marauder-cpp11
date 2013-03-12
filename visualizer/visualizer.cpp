// See LICENSE file for copyright and license details.

#include "visualizer/visualizer.hpp"
#include <cmath>
#include <cassert>
#include <stdexcept>
#include "core/jsonHelpers.hpp"
#include "core/command.hpp"
#include "core/command/commandMove.hpp"
#include "core/command/commandAttack.hpp"
#include "core/command/commandEndTurn.hpp"
#include "core/player.hpp"
#include "core/eventView.hpp"
#include "visualizer/math.hpp"
#include "visualizer/vertexArray.hpp"
#include "visualizer/gl.hpp"
#include "visualizer/event/eventEndTurnVisualizer.hpp"
#include "visualizer/event/eventAttackVisualizer.hpp"
#include "visualizer/event/eventMoveVisualizer.hpp"
#include "visualizer/meshBuilders.hpp"
#include "visualizer/tilePicker.hpp"

// public:

Visualizer::Visualizer(Core& core)
  : mCore(core),
    mConfig(parseJsonFile("confVisualizer.json")),
    mPathToData(mConfig["pathToData"].asString()),
    mMode(Mode::Normal),
    mSDLFlags(SDL_OPENGL | SDL_RESIZABLE),
    mBitsPerPixel(mConfig["bitsPerPixel"].asInt()),
    mWinSize(JsonValueToV2i(mConfig["resolution"])),
    mMousePosition(0, 0),
    mActiveTilePosition(0, 0),
    mIsRotatingCamera(false),
    mDone(false),
    mCurrentEventVisualizer(nullptr),
    mTilePicker(new TilePicker(*this, camera())) // TODO: memleak
{
  SDL_Init(SDL_INIT_EVERYTHING);
  mScreen = SDL_SetVideoMode(mWinSize.x(), mWinSize.y(),
      mBitsPerPixel, mSDLFlags);
  initOpengl();
  mFloorTexture = loadTexture(mPathToData + "floor.png");
  initCamera();
  loadUnitResources();
  initVertexArrays();
  createSceneManagerForEachPlayer();
  createUnitSceneNodes();
}

Visualizer::~Visualizer() {
  if (mCurrentEventVisualizer) {
    delete mCurrentEventVisualizer;
    mCurrentEventVisualizer = nullptr;
  }
  SDL_Quit();
}

Core& Visualizer::core() {
  return mCore;
}

const Core& Visualizer::core() const {
  return mCore;
}

SceneManager& Visualizer::sceneManager() {
  return mSceneManagers.at(core().currentPlayer().id);
}

const SceneManager& Visualizer::sceneManager() const {
  return mSceneManagers.at(core().currentPlayer().id);
}

void Visualizer::cleanWalkableMapArray() {
  mVaWalkableMap = VertexArray();
}

void Visualizer::rebuildWalkableMapArray() {
  mVaWalkableMap = buildWalkableArray(*this);
}

void Visualizer::rebuildMapArray() {
  mVaMap = buildMapArray(*this, mFloorTexture);
}

void Visualizer::run() {
  mainloop();
}

V2f Visualizer::v2iToV2f(const V2i& i) const {
  assert(core().map().isInboard(i));
  return V2f(i.x(), i.y());
}

V2f Visualizer::indexToCircleVertex(int count, int i) const {
  float n = M_PI_2 + 2 * M_PI * i / count;
  return V2f(std::cos(n), std::sin(n)) * 0.5f;
}

void Visualizer::createUnitSceneNodes() {
  for (const Unit* unit : core().units()) {
    createUnitNode(*unit);
  }
}

// private:

void Visualizer::processSDLEvent(const SDL_MouseMotionEvent& e) {
  mMousePosition = V2i(static_cast<int>(e.x), static_cast<int>(e.y));
  if (mIsRotatingCamera) {
    camera().rotateAroundZAxis(-e.xrel);
    camera().rotateAroundXAxis(-e.yrel);
  }
}

void Visualizer::processSDLEvent(const SDL_KeyboardEvent& e) {
  switch (e.keysym.sym) {
  case SDLK_ESCAPE:
  case SDLK_q:
    mDone = true;
    break;
  case SDLK_c:
    centerCameraOnSelectedUnit();
    break;
  case SDLK_t:
    switchActiveTileType();
    break;
  case SDLK_e:
    core().doCommand(CommandEndTurn());
    break;
  case SDLK_u:
    createNewUnitInActiveTile();
    break;
  case SDLK_d:
    camera().rotateAroundZAxis(15);
    break;
  case SDLK_a:
    camera().rotateAroundZAxis(-15);
    break;
  case SDLK_w:
    camera().zoomIn(1);
    break;
  case SDLK_s:
    camera().zoomOut(1);
    break;
  case SDLK_UP:
    camera().move(0);
    break;
  case SDLK_DOWN:
    camera().move(180);
    break;
  case SDLK_LEFT:
    camera().move(270);
    break;
  case SDLK_RIGHT:
    camera().move(90);
    break;
  default:
    std::printf("processKeyDownEvent(): "
        "Unknown key (%d, 0x%x) was pressed.\n",
        e.keysym.sym, e.keysym.sym);
    break;
  }
}

void Visualizer::processSDLEvent(const SDL_ResizeEvent& e) {
  mWinSize = V2i(e.w, e.h);
  mScreen = SDL_SetVideoMode(mWinSize.x(), mWinSize.y(),
      mBitsPerPixel, mSDLFlags);
  initOpengl();
}

void Visualizer::processSDLEventButtonUp(const SDL_MouseButtonEvent& e) {
  if (e.button == SDL_BUTTON_RIGHT) {
    mIsRotatingCamera = false;
  }
  if (e.button == SDL_BUTTON_WHEELUP) {
    camera().zoomIn(1);
  } else if (e.button == SDL_BUTTON_WHEELDOWN) {
    camera().zoomOut(1);
  }
}

void Visualizer::processSDLEventButtonDown(const SDL_MouseButtonEvent& e) {
  if (e.button == SDL_BUTTON_RIGHT) {
    mIsRotatingCamera = true;
  } else if (e.button == SDL_BUTTON_LEFT) {
    processClickOnTile();
  }
}

void Visualizer::processSDLEvent(const SDL_Event& e) {
  switch (e.type) {
  case SDL_QUIT:
    mDone = true;
    break;
  case SDL_VIDEORESIZE:
    processSDLEvent(e.resize);
    break;
  case SDL_KEYDOWN:
    processSDLEvent(e.key);
    break;
  case SDL_MOUSEMOTION:
    processSDLEvent(e.motion);
    break;
  case SDL_MOUSEBUTTONUP:
    processSDLEventButtonUp(e.button);
    break;
  case SDL_MOUSEBUTTONDOWN:
    processSDLEventButtonDown(e.button);
    break;
  default:
    break;
  }
}

void Visualizer::processClickOnFriendlyUnit(Unit& unit) {
  core().setSelectedUnit(unit);
  core().pathfinder().fillMap(core().selectedUnit());
  rebuildWalkableMapArray();
}

void Visualizer::processClickOnEnemyUnit(Unit& unit) {
  const V2i& from = core().selectedUnit().position();
  const V2i& to = unit.position();
  bool isLineOfSightClear = core().isLineOfSightClear(from, to);
  if (isLineOfSightClear
      && core().selectedUnit().actionPoints() >= 3)
  {
    CommandAttack cmd(core().selectedUnit().id(), unit.id());
    core().doCommand(cmd);
  }
}

void Visualizer::processClickOnUnit(Unit& unit) {
  if (unit.playerID() == core().currentPlayer().id) {
    processClickOnFriendlyUnit(unit);
  } else if (core().isAnyUnitSelected()) {
    processClickOnEnemyUnit(unit);
  }
}

void Visualizer::processClickOnEmptyTile(Tile& tile) {
  int actionPoints = core().selectedUnit().actionPoints();
  if (tile.cost <= actionPoints && tile.parent.value() != DirectionID::NONE) {
    if (core().map().tile(mActiveTilePosition).cost <= actionPoints) {
      CommandMove cmd(core().selectedUnit().id(), mActiveTilePosition);
      core().doCommand(cmd);
    }
  }
}

void Visualizer::processClickOnTile() {
  if (mMode != Mode::Normal) {
    return;
  }
  if (core().isUnitAt(mActiveTilePosition)) {
    Unit& unit = core().unitAt(mActiveTilePosition);
    processClickOnUnit(unit);
  } else if (core().isAnyUnitSelected()) {
    Tile& tile = core().map().tile(mActiveTilePosition);
    processClickOnEmptyTile(tile);
  }
}

void Visualizer::sdlEvents() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    processSDLEvent(e);
  }
}

void Visualizer::centerCameraOnSelectedUnit() {
  if (!core().isAnyUnitSelected()) {
    return;
  }
  V2f unitPosition = v2iToV2f(core().selectedUnit().position());
  camera().setPosition(unitPosition);
}

void Visualizer::switchActiveTileType() {
  Tile& t = core().map().tile(mActiveTilePosition);
  t.obstacle = !t.obstacle;
  rebuildMapArray();
  mVaObstacles = buildObstaclesArray(*this, mFloorTexture);
  core().calculateFow();
  rebuildMapArray();
  if (core().isAnyUnitSelected()) {
    core().pathfinder().fillMap(core().selectedUnit());
    rebuildWalkableMapArray();
  }
}

void Visualizer::createNewUnitInActiveTile() {
  core().addUnit(
        mActiveTilePosition,
        core().currentPlayer().id,
        core().unitType("truck"),
        Direction(DirectionID::NE));
  if (core().isAnyUnitSelected()) {
    rebuildWalkableMapArray();
  }
  rebuildMapArray();
  createUnitNode(core().unitAt(mActiveTilePosition));
}

void Visualizer::createSceneManagerForEachPlayer() {
  for (const Player* player : mCore.players()) {
    mSceneManagers[player->id] = SceneManager();
  }
}

void Visualizer::scrollMap() {
  const V2i& p = mMousePosition;
  int offset = 15;
  if (p.x() < offset) {
    camera().move(270);
  } else if(p.x() > mScreen->w - offset) {
    camera().move(90);
  }
  if (p.y() < offset) {
    camera().move(0);
  } else if(p.y() > mScreen->h - offset) {
    camera().move(180);
  }
}

void Visualizer::updateActiveTilePosition() {
  mActiveTilePosition = mTilePicker->pick(mMousePosition);
}

void Visualizer::mainloop() {
  while (!mDone) {
    sdlEvents();
    logic();
    scrollMap();
    updateActiveTilePosition();
    draw();
    // SDLDelay(1000.0f / 24.0f);
  }
}

void Visualizer::initOpengl() {
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  glViewport(0, 0, mWinSize.x(), mWinSize.y());
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  GLdouble left = 0.5;
  GLdouble right = -0.5;
  GLdouble bottom = -0.5 * aspectRatio();
  GLdouble top = 0.5 * aspectRatio();
  GLdouble near = 1.0;
  GLdouble far = 500.0;
  glFrustum(left, right, bottom, top, near, far);
  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_BLEND);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_COLOR_MATERIAL);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0.0f);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Visualizer::initCamera() {
  camera().setMaxXAxisAngle(50.0f);
  camera().setMinXAxisAngle(0.0f);
  camera().setXAxisAngle(45.0f);
  camera().setZAxisAngle(45.0f);
  camera().setMaxPosition(v2iToV2f(core().map().size() - 1));
  camera().setPosition(V2f(0.0f, 0.0f));
  camera().setMaxZoom(50.0f);
  camera().setMinZoom(3.0f);
  camera().setZoom(20.0f);
}

void Visualizer::initVertexArrays() {
  rebuildMapArray();
  mVaObstacles = buildObstaclesArray(*this, mFloorTexture);
  buildUnitCircles();
}

void Visualizer::createUnitNode(const Unit& unit) {
  for (const Player* player : core().players()) {
    auto& sm = mSceneManagers.at(player->id); // shortcut
    auto* unitNode = new SceneNode();
    unitNode->mVertexArray = &mVaUnits[unit.type().id];
    unitNode->mPosition = v2iToV2f(unit.position());
    unitNode->mRotationAngle = dirToAngle(unit.direction());
    sm.addNode(unit.id(), unitNode);
    auto* circleNode = new SceneNode();
    circleNode->mVertexArray = &(mVaUnitCircles.at(unit.playerID()));
    unitNode->mChildrens.push_back(circleNode);
  }
}

void Visualizer::buildUnitCircles(){
  std::vector<Color> colors = {
    Color(1.0f, 0.0f, 0.0f),
    Color(0.0f, 0.0f, 1.0f)
  };
  for (const Color& color : colors) {
    mVaUnitCircles.push_back(
        buildUnitCircleVertexArray(*this, 0.5f, color));
  }
}

void Visualizer::loadUnitResources() {
  Json::Value resources = parseJsonFile("unitResources.json");
  for (const std::string& key : resources.getMemberNames()) {
    unsigned int id = core().unitType(key).id;
    const Json::Value& unitInfo = resources[key];
    std::string texturePath = mPathToData + unitInfo["textureName"].asString();
    std::string objModelPath = mPathToData + unitInfo["objModelName"].asString();
    mVaUnits[id] = ObjModel(objModelPath).build();
    mVaUnits[id].setTextureID(loadTexture(texturePath));
  }
}

void Visualizer::drawMap() {
  mVaMap.draw();
  mVaObstacles.draw();
  mVaWalkableMap.draw();
}

void Visualizer::drawSelectedunitMarker() {
  const Unit& u = core().selectedUnit();
  V2f p = v2iToV2f(u.position());
  VertexArray v(Color(1.0f, 0.0f, 0.0f), PrimitiveType::Lines);
  float sn = std::sin(SDL_GetTicks() / 100.0f) / 4.0f;
  v.addVertex(V3f(p, sn + 1.0f));
  v.addVertex(V3f(p, sn + 1.5f));
  glLineWidth(2);
  v.draw();
  glLineWidth(1);
}

void Visualizer::draw() {
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  camera().set();
  drawMap();
  sceneManager().draw();
  if (mMode == Mode::ShowEvent) {
    currentEventVisualizer().draw();
  }
  if (core().isAnyUnitSelected()) {
    drawSelectedunitMarker();
  }
  SDL_GL_SwapBuffers();
}

void Visualizer::logic() {
  if (mMode == Mode::Normal) {
    PlayerID id = core().currentPlayer().id;
    std::list<EventView*>& eventViewList = core().eventViewList(id);
    if (!eventViewList.empty()) {
      const EventView& eventView = *eventViewList.front(); // TODO: fix memory leak
      eventViewList.pop_front();
      mCurrentEventVisualizer = newEventVisualizer(*this, eventView);
      assert(mCurrentEventVisualizer);
      mMode = Mode::ShowEvent;
    }
  }
  if (mMode == Mode::ShowEvent && currentEventVisualizer().isFinished()) {
    mMode = Mode::Normal;
    currentEventVisualizer().end();
    assert(mCurrentEventVisualizer);
    delete mCurrentEventVisualizer;
    mCurrentEventVisualizer = nullptr;
  }
}

float Visualizer::aspectRatio() const {
  float x = mWinSize.x();
  float y = mWinSize.y();
  return y / x;
}

Camera& Visualizer::camera() {
  return mCamera;
}

EventVisualizer& Visualizer::currentEventVisualizer() {
  if (!mCurrentEventVisualizer) {
    throw std::logic_error("no current event visualizer");
  }
  return *mCurrentEventVisualizer;
}
