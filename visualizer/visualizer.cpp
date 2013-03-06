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

// public:

Visualizer::Visualizer(Core& core)
  : mCore(core),
    mConfig(parseJsonFile("confVisualizer.json")),
    mPathToData(mConfig["pathToData"].asString()),
    mTileSize(1.0f),
    mHexEx(tileSize() / 2.0f),
    mHexIn(std::sqrt(std::pow(mHexEx, 2) - std::pow(mHexEx / 2.0f, 2))),
    mMode(Mode::Normal),
    mSDLFlags(SDL_OPENGL | SDL_RESIZABLE),
    mBitsPerPixel(mConfig["bitsPerPixel"].asInt()),
    mWinSize(JsonValueToV2i(mConfig["resolution"])),
    mMousePos(0, 0),
    mActiveTilePos(0, 0),
    mIsRotatingCamera(false),
    mDone(false),
    mCurrentEventVisualizer(nullptr)
{
  SDL_Init(SDL_INIT_EVERYTHING);
  mScreen = SDL_SetVideoMode(mWinSize.x(), mWinSize.y(),
      mBitsPerPixel, mSDLFlags);
  initOpengl();
  mFloorTexture = loadTexture(mPathToData + "floor.png");
  initCamera();
  loadUnitResources();
  initVertexArrays();
  // TODO: Move to createSceneManagerForEachPlayer()
  for (const Player* player : mCore.players()) {
    mSceneManagers[player->id] = SceneManager();
  }
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
  mVaWalkableMap = buildWalkableArray();
}

void Visualizer::rebuildMapArray() {
  mVaMap = buildMapArray();
}

void Visualizer::run() {
  mainloop();
}

V2f Visualizer::v2iToV2f(const V2i& i) const {
  assert(core().map().isInboard(i));
  V2f v(i.x() * mHexIn * 2, i.y() * mHexEx * 1.5);
  if (i.y() % 2 == 0) {
    v.setX(v.x() + mHexIn);
  }
  return v;
}

V2f Visualizer::indexToCircleVertex(int count, int i) const {
  float n = M_PI_2 + 2 * M_PI * i / count;
  return V2f(std::cos(n), std::sin(n)) * mHexEx;
}

V2f Visualizer::indexToHexVertex(int i) const {
  return indexToCircleVertex(6, i);
}

float Visualizer::tileSize() const {
  return mTileSize;
}

void Visualizer::createUnitSceneNodes() {
  for (const Unit* unit : core().units()) {
    createUnitNode(*unit);
  }
}

// private:

void Visualizer::processSDLEvent(const SDL_MouseMotionEvent& e) {
  mMousePos = V2i(static_cast<int>(e.x), static_cast<int>(e.y));
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
  mVaWalkableMap = buildWalkableArray();
}

void Visualizer::processClickOnEnemyUnit(Unit& unit) {
  const V2i& from = core().selectedUnit().position();
  const V2i& to = unit.position();
  bool isLosClear = core().isLosClear(from, to);
  if (isLosClear
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
  if (tile.cost <= actionPoints && tile.parent.value() != DirID::NONE) {
    if (core().map().tile(mActiveTilePos).cost <= actionPoints) {
      CommandMove cmd(core().selectedUnit().id(), mActiveTilePos);
      core().doCommand(cmd);
    }
  }
}

void Visualizer::processClickOnTile() {
  if (mMode != Mode::Normal) {
    return;
  }
  if (core().isUnitAt(mActiveTilePos)) {
    Unit& unit = core().unitAt(mActiveTilePos);
    processClickOnUnit(unit);
  } else if (core().isAnyUnitSelected()) {
    Tile& tile = core().map().tile(mActiveTilePos);
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
  V2f unitPos = v2iToV2f(core().selectedUnit().position());
  camera().setPos(unitPos);
}

void Visualizer::switchActiveTileType() {
  Tile& t = core().map().tile(mActiveTilePos);
  t.obstacle = !t.obstacle;
  mVaMap = buildMapArray();
  mVaObstacles = buildObstaclesArray();
  core().calculateFow();
  rebuildMapArray();
  if (core().isAnyUnitSelected()) {
    core().pathfinder().fillMap(core().selectedUnit());
    mVaWalkableMap = buildWalkableArray();
  }
}

void Visualizer::createNewUnitInActiveTile() {
  core().addUnit(
        mActiveTilePos,
        core().currentPlayer().id,
        core().unitType("truck"),
        Dir(DirID::NE));
  if (core().isAnyUnitSelected()) {
    core().pathfinder().fillMap(core().selectedUnit());
    mVaWalkableMap = buildWalkableArray();
  }
  rebuildMapArray();
  createUnitNode(core().unitAt(mActiveTilePos));
}

VertexArray Visualizer::buildPickingTilesArray() {
  VertexArray v;
  core().map().forEachPos([&](const V2i& p) {
    Color3u color(p.x(), p.y(), 1);
    V2f pos = v2iToV2f(p);
    for (int i = 0; i < 6; ++i) {
      v.addVertex(
          V3f(pos + indexToHexVertex(i)),
          color);
      v.addVertex(
          V3f(pos + indexToHexVertex(i + 1)),
          color);
      v.addVertex(
          V3f(pos),
          color);
    }
  });
  return v;
}

Color3u Visualizer::fowColor(const V2i& position) {
  GLuint n;
  if (core().map().tile(position).fow == 0) {
    n = 180; // dark
  } else {
    n = 255; // bright
  }
  return Color3u(n);
}

VertexArray Visualizer::buildMapArray() {
  VertexArray v;
  v.setTextureID(mFloorTexture);
  core().map().forEachPos([&](const V2i& p) {
    V2f pos = v2iToV2f(p);
    Color3u color = fowColor(p);
    for (int i = 0; i < 6; ++i) {
      v.addVertex(
          pos + indexToHexVertex(i),
          V2f(0.0f, 0.0f),
          color);
      v.addVertex(
          pos + indexToHexVertex(i + 1),
          V2f(1.0f, 0.0f),
          color);
      v.addVertex(
          pos,
          V2f(0.5f, 0.5f),
          color);
    }
  });
  return v;
}

VertexArray Visualizer::buildObstaclesArray() {
  VertexArray v(Color(0.4f, 0.1f, 0.0f));
  v.setTextureID(mFloorTexture);
  core().map().forEachPos([&](const V2i& p) {
    if (core().map().tile(p).obstacle) {
      V2f pos = v2iToV2f(p);
      for (int i = 0; i < 6; ++i) {
        v.addVertex(
            V3f(pos + indexToHexVertex(i) * 0.7f, 0.01f),
            V2f(0.0f, 0.0f));
        v.addVertex(
            V3f(pos + indexToHexVertex(i + 1) * 0.7f, 0.01f),
            V2f(1.0f, 0.0f));
        v.addVertex(
            V3f(pos, 0.01f),
            V2f(0.5f, 0.5f));
      }
    }
  });
  return v;
}

VertexArray Visualizer::buildWalkableArray() {
  VertexArray v(Color(0.0f, 0.0f, 1.0f), PrimitiveType::Lines);
  core().map().forEachPos([&](const V2i& p) {
    const Tile& t = core().map().tile(p);
    if (t.parent.value() != DirID::NONE && t.cost < 50) {
      V2i to = Dir::getNeighbourPos(p, t.parent);
      if (core().map().isInboard(to)) {
        V2f fromF = v2iToV2f(p);
        V2f toF = v2iToV2f(to);
        v.addVertex(V3f(fromF, 0.01f));
        v.addVertex(V3f(toF, 0.01f));
      }
    }
  });
  return v;
}

V2i Visualizer::pickTile(const V2i& mousePos) {
  GLint viewport[4];
  GLubyte pixel[3];
  glGetIntegerv(GL_VIEWPORT, viewport);
  viewport[3] -= 1;
  void* pixelPointer = &pixel;
  glReadPixels(mousePos.x(), viewport[3] - mousePos.y(),
      1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixelPointer);
  return V2i(pixel[0], pixel[1]);
}

void Visualizer::drawForPicking() {
  glLoadIdentity();
  camera().set();
  mVaPick.draw();
}

void Visualizer::scrollMap() {
  const V2i& p = mMousePos;
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

void Visualizer::pickTile() {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawForPicking();
  mActiveTilePos = pickTile(mMousePos);
}

void Visualizer::mainloop() {
  while (!mDone) {
    sdlEvents();
    logic();
    scrollMap();
    pickTile();
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
  camera().setMaxPos(v2iToV2f(core().map().size() - 1));
  camera().setPos(V2f(0.0f, 0.0f));
  camera().setMaxZoom(50.0f);
  camera().setMinZoom(3.0f);
  camera().setZoom(20.0f);
}

void Visualizer::initVertexArrays() {
  mVaPick = buildPickingTilesArray();
  mVaMap = buildMapArray();
  mVaObstacles = buildObstaclesArray();
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

VertexArray Visualizer::buildUnitCircleVertexArray(const Color& color) {
  VertexArray v(color, PrimitiveType::Lines);
  const int verticesCount = 12;
  for (int i = 0; i < verticesCount; ++i) {
    const float k = mHexIn * 2.0f; // resize coefficient
    const float h = 0.01f;
    v.addVertex(V3f(
        indexToCircleVertex(verticesCount, i) * k, h));
    v.addVertex(V3f(
        indexToCircleVertex(verticesCount, i + 1) * k, h));
  }
  return v;
}

void Visualizer::buildUnitCircles(){
  std::vector<Color> colors = {
    Color(1.0f, 0.0f, 0.0f),
    Color(0.0f, 0.0f, 1.0f)
  };
  for (const Color& color : colors) {
    mVaUnitCircles.push_back(buildUnitCircleVertexArray(color));
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
  v.addVertex(V3f(p, sn + tileSize()));
  v.addVertex(V3f(p, sn + tileSize() * 1.5f));
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
