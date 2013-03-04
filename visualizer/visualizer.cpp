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
    mEventVisualizer(nullptr),
    mSceneManager()
{
  SDL_Init(SDL_INIT_EVERYTHING);
  setScreen(SDL_SetVideoMode(winSize().x(), winSize().y(),
      mBitsPerPixel, mSDLFlags));
  initOpengl();
  setFloorTexture(loadTexture(mPathToData + "floor.png"));
  initCamera();
  loadUnitResources();
  initVertexArrays();
  recreateUnitSceneNodes();
}

Visualizer::~Visualizer() {
  if (mEventVisualizer) {
    delete mEventVisualizer;
    mEventVisualizer = nullptr;
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
  return mSceneManager;
}

const SceneManager& Visualizer::sceneManager() const {
  return mSceneManager;
}

const V2i& Visualizer::winSize() const {
  return mWinSize;
}

Visualizer::Mode Visualizer::mode() const {
  return mMode;
}

Camera& Visualizer::camera() {
  return mCamera;
}

const V2i& Visualizer::activeTilePos() const {
  return mActiveTilePos;
}

bool Visualizer::isRotatingCamera() const {
  return mIsRotatingCamera;
}

const V2i& Visualizer::mousePos() const {
  return mMousePos;
}

SDL_Surface* Visualizer::screen() {
  return mScreen;
}

float Visualizer::tileSize() const {
  return mTileSize;
}

bool Visualizer::done() const {
  return mDone;
}

void Visualizer::setDone(bool done) {
  mDone = done;
}

void Visualizer::setWinSize(const V2i& winSize) {
  mWinSize = winSize;
}

void Visualizer::setActiveTilePos(const V2i& activeTilePos) {
  mActiveTilePos = activeTilePos;
}

void Visualizer::setMousePos(const V2i& mousePos) {
  mMousePos = mousePos;
}

void Visualizer::setMode(Mode mode) {
  mMode = mode;
}

void Visualizer::setIsRotatingCamera(bool isRotatingCamera) {
  mIsRotatingCamera = isRotatingCamera;
}

void Visualizer::setFloorTexture(int textureID) {
  mFloorTexture = textureID;
}

void Visualizer::setScreen(SDL_Surface* screen) {
  mScreen = screen;
}

void Visualizer::setVaWalkableMap(const VertexArray& va) {
  mVaWalkableMap = va;
}

void Visualizer::cleanWalkableMapArray() {
  setVaWalkableMap(VertexArray());
}

void Visualizer::rebuildWalkableMapArray() {
  setVaWalkableMap(buildWalkableArray());
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

V2f Visualizer::indexToCircleVertex(int count, int i) {
  float n = M_PI_2 + 2 * M_PI * i / count;
  return V2f(std::cos(n), std::sin(n)) * mHexEx;
}

V2f Visualizer::indexToHexVertex(int i) {
  return indexToCircleVertex(6, i);
}

float Visualizer::aspectRatio() const {
  float x = winSize().x();
  float y = winSize().y();
  return y / x;
}

VertexArray Visualizer::buildMapArray() {
  VertexArray v;
  v.mTextureID = mFloorTexture;
  core().map().forEachPos([&](const V2i& p) {
    V2f pos = v2iToV2f(p);
    for (int i = 0; i < 6; ++i) {
      appendV3f(&v.vertices, pos + indexToHexVertex(i));
      appendV3f(&v.vertices, pos + indexToHexVertex(i + 1));
      appendV3f(&v.vertices, pos);
      appendV2f(&v.textureCoordinates, V2f(0.0f, 0.0f));
      appendV2f(&v.textureCoordinates, V2f(1.0f, 0.0f));
      appendV2f(&v.textureCoordinates, V2f(0.5f, 0.5f));
      for (int tmp = 0; tmp < 3; ++tmp) {
        if (core().map().tile(p).fow == 0) {
          appendRGB(&v.colors, 180, 180, 180);
        } else {
          appendRGB(&v.colors, 255, 255, 255);
        }
      }
    }
  });
  return v;
}

VertexArray Visualizer::buildObstaclesArray() {
  VertexArray v(Color(0.4f, 0.1f, 0.0f));
  v.mTextureID = mFloorTexture;
  core().map().forEachPos([&](const V2i& p) {
    if (core().map().tile(p).obstacle) {
      V2f pos = v2iToV2f(p);
      for (int i = 0; i < 6; ++i) {
        appendV3f(&v.vertices, V3f(pos + indexToHexVertex(i) * 0.7f, 0.01f));
        appendV3f(&v.vertices, V3f(pos + indexToHexVertex(i + 1) * 0.7f, 0.01f));
        appendV3f(&v.vertices, V3f(pos, 0.01f));
        appendV2f(&v.textureCoordinates, V2f(0.0f, 0.0f));
        appendV2f(&v.textureCoordinates, V2f(1.0f, 0.0f));
        appendV2f(&v.textureCoordinates, V2f(0.5f, 0.5f));
      }
    }
  });
  return v;
}

VertexArray Visualizer::buildWalkableArray() {
  VertexArray v(Color(0.0f, 0.0f, 1.0f), PrimitiveType::Lines);
  core().map().forEachPos([&](const V2i& p) {
    Tile& t = core().map().tile(p);
    if (t.parent.value() != DirID::NONE && t.cost < 50) {
      V2i to = Dir::getNeighbourPos(p, t.parent);
      if (core().map().isInboard(to)) {
        V2f fromF = v2iToV2f(p);
        V2f toF = v2iToV2f(to);
        appendV3f(&v.vertices, V3f(fromF, 0.01f));
        appendV3f(&v.vertices, V3f(toF, 0.01f));
      }
    }
  });
  return v;
}

void Visualizer::drawMap() {
  mVaMap.draw();
  mVaObstacles.draw();
  mVaWalkableMap.draw();
}

void Visualizer::recreateUnitSceneNodes() {
  for (auto& pair : mSceneManager.nodes()) {
    SceneNode* node = pair.second;
    assert(node);
    delete node;
  }
  for (const Unit* pUnit : core().units()) {
    const Unit& unit = *pUnit;
    createUnitNode(unit);
  }
}

void Visualizer::drawSelectedunitMarker() {
  const Unit& u = core().selectedUnit();
  V2f p = v2iToV2f(u.position());
  VertexArray v(Color(1.0f, 0.0f, 0.0f), PrimitiveType::Lines);
  float sn = std::sin(SDL_GetTicks() / 100.0f) / 4.0f;
  appendV3f(&v.vertices, V3f(p, sn + tileSize()));
  appendV3f(&v.vertices, V3f(p, sn + tileSize() * 1.5f));
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
  mSceneManager.draw();
  if (mode() == Mode::ShowEvent) {
    assert(mEventVisualizer);
    mEventVisualizer->draw();
  }
  if (core().isAnyUnitSelected()) {
    drawSelectedunitMarker();
  }
  SDL_GL_SwapBuffers();
}

void Visualizer::processClickOnFriendlyUnit(Unit& unit) {
  core().setSelectedUnit(unit);
  core().pathfinder().fillMap(core().selectedUnit());
  setVaWalkableMap(buildWalkableArray());
}

void Visualizer::processClickOnEnemyUnit(Unit& unit) {
  const V2i& from = core().selectedUnit().position();
  const V2i& to = unit.position();
  bool isLosClear = core().isLosClear(from, to);
  if (isLosClear) {
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
    if (core().map().tile(activeTilePos()).cost <= actionPoints) {
      CommandMove cmd(core().selectedUnit().id(), activeTilePos());
      core().doCommand(cmd);
    }
  }
}

void Visualizer::processClickOnTile() {
  if (mode() != Mode::Normal) {
    return;
  }
  if (core().isUnitAt(activeTilePos())) {
    Unit& unit = core().unitAt(activeTilePos());
    processClickOnUnit(unit);
  } else if (core().isAnyUnitSelected()) {
    Tile& tile = core().map().tile(activeTilePos());
    processClickOnEmptyTile(tile);
  }
}

void Visualizer::processSDLEvent(const SDL_MouseMotionEvent& e) {
  setMousePos(V2i(static_cast<int>(e.x), static_cast<int>(e.y)));
  if (isRotatingCamera()) {
    camera().rotateAroundZAxis(-e.xrel);
    camera().rotateAroundXAxis(-e.yrel);
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
  Tile& t = core().map().tile(activeTilePos());
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
        activeTilePos(),
        core().currentPlayer().id,
        core().getUnitType("truck"),
        Dir(DirID::NE));
  if (core().isAnyUnitSelected()) {
    core().pathfinder().fillMap(core().selectedUnit());
    mVaWalkableMap = buildWalkableArray();
  }
  rebuildMapArray();
  createUnitNode(core().unitAt(activeTilePos()));
}

void Visualizer::processSDLEvent(const SDL_KeyboardEvent& e) {
  switch (e.keysym.sym) {
  case SDLK_ESCAPE:
  case SDLK_q:
    setDone(true);
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
    camera().zoomIn(10);
    break;
  case SDLK_s:
    camera().zoomOut(10);
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
  setWinSize(V2i(e.w, e.h));
  setScreen(SDL_SetVideoMode(winSize().x(), winSize().y(),
      mBitsPerPixel, mSDLFlags));
  initOpengl();
}

void Visualizer::processSDLEventButtonUp(const SDL_MouseButtonEvent& e) {
  if (e.button == SDL_BUTTON_RIGHT) {
    setIsRotatingCamera(false);
  }
  if (e.button == SDL_BUTTON_WHEELUP) {
    camera().zoomIn(1);
  } else if (e.button == SDL_BUTTON_WHEELDOWN) {
    camera().zoomOut(1);
  }
}

void Visualizer::processSDLEventButtonDown(const SDL_MouseButtonEvent& e) {
  if (e.button == SDL_BUTTON_RIGHT) {
    setIsRotatingCamera(true);
  } else if (e.button == SDL_BUTTON_LEFT) {
    processClickOnTile();
  }
}

// TODO: delete me!
namespace {

// tmp func
EventView* basicConvertEventToEventView(const Event& event) {
  switch (event.type()) {
  case EventTypeID::Move:
    return new EventMoveView(dynamic_cast<const EventMove&>(event));
  case EventTypeID::EndTurn:
    return new EventEndTurnView(dynamic_cast<const EventEndTurn&>(event));
  case EventTypeID::Attack:
    return new EventAttackView(dynamic_cast<const EventAttack&>(event));
  default:
    throw std::logic_error("default case!");
  }
}

} // namespace

void Visualizer::screenScenarioMainEvents() {
  core().eventManager().switchToNextEvent();
  if (mEventVisualizer) {
    delete mEventVisualizer;
  }
  // TODO: Rewrite this
  EventView* eventView = basicConvertEventToEventView(
      core().eventManager().currentEvent());
  mEventVisualizer = newEventVisualizer(*this, *eventView);
  assert(mEventVisualizer);
  setMode(Mode::ShowEvent);
}

void Visualizer::logic() {
  while (mode() == Mode::Normal && core().eventManager().unshownEventsLeft()) {
    screenScenarioMainEvents();
  }
  if (mode() == Mode::ShowEvent) {
    if (mEventVisualizer->isFinished()) {
      core().eventManager().applyCurrentEvent();
      setMode(Mode::Normal);
      mEventVisualizer->end();
    }
  }
}

void Visualizer::processSDLEvent(const SDL_Event& e) {
  switch (e.type) {
  case SDL_QUIT:
    setDone(true);
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

void Visualizer::sdlEvents() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    processSDLEvent(e);
  }
}

VertexArray Visualizer::buildPickingTilesArray() {
  VertexArray v;
  core().map().forEachPos([&](const V2i& p) {
    V2f pos = v2iToV2f(p);
    for (int i = 0; i < 6; ++i) {
      appendV3f(&v.vertices, V3f(pos + indexToHexVertex(i)));
      appendV3f(&v.vertices, V3f(pos + indexToHexVertex(i + 1)));
      appendV3f(&v.vertices, V3f(pos));
      appendRGB(&v.colors, p.x(), p.y(), 1);
      appendRGB(&v.colors, p.x(), p.y(), 1);
      appendRGB(&v.colors, p.x(), p.y(), 1);
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
  const V2i& p = mousePos();
  int offset = 15;
  if (p.x() < offset) {
    camera().move(270);
  } else if(p.x() > screen()->w - offset) {
    camera().move(90);
  }
  if (p.y() < offset) {
    camera().move(0);
  } else if(p.y() > screen()->h - offset) {
    camera().move(180);
  }
}

void Visualizer::pickTile() {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawForPicking();
  setActiveTilePos(pickTile(mMousePos));
}

void Visualizer::mainloop() {
  while (!done()) {
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
  glViewport(0, 0, winSize().x(), winSize().y());
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
  buildUnitCirclesVertexArrays();
}

void Visualizer::createUnitNode(const Unit& unit) {
  auto* node = new SceneNode();
  node->mVertexArray = &mVaUnits[unit.type().id];
  node->mPosition = v2iToV2f(unit.position());
  node->mRotationAngle = dirToAngle(unit.direction());
  mSceneManager.addNode(unit.id(), node);
  {
    auto* node2 = new SceneNode();
    node2->mVertexArray = &(mVaUnitCircles.at(unit.playerID()));
    node->mChildrens.push_back(node2);
  }
}

void Visualizer::buildUnitCirclesVertexArrays(){
    std::vector<Color> colors = {
      Color(1.0f, 0.0f, 0.0f),
      Color(0.0f, 0.0f, 1.0f)
    };
    for (const Color& color : colors) {
      VertexArray v(PrimitiveType::Lines);
      v.mHaveColor = true;
      v.mColor = color;
      const int verticesCount = 12;
      for (int i = 0; i < verticesCount; ++i) {
        const float k = mHexIn * 2.0f; // resize koefficient
        const float h = 0.01f;
        appendV3f(&v.vertices, V3f(
            indexToCircleVertex(verticesCount, i) * k, h));
        appendV3f(&v.vertices, V3f(
            indexToCircleVertex(verticesCount, i + 1) * k, h));
      }
      mVaUnitCircles.push_back(v);
    }
}

void Visualizer::loadUnitResources() {
  Json::Value resources = parseJsonFile("unitResources.json");
  for (const std::string& key : resources.getMemberNames()) {
    unsigned int id = core().getUnitType(key).id;
    const Json::Value& unitInfo = resources[key];
    std::string texturePath = mPathToData + unitInfo["textureName"].asString();
    std::string objModelPath = mPathToData + unitInfo["objModelName"].asString();
    mVaUnits[id] = ObjModel(objModelPath).build();
    mVaUnits[id].mTextureID = loadTexture(texturePath);
  }
}
