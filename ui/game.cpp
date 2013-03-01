// See LICENSE file for copyright and license details.

#include "ui/game.hpp"
#include <cmath>
#include <cassert>
#include <stdexcept>
#include "core/jsonHelpers.hpp"
#include "ui/math.hpp"
#include "ui/vertexArray.hpp"
#include "ui/gl.hpp"
#include "ui/event/eventEndTurnVisualizer.hpp"
#include "ui/event/eventAttackVisualizer.hpp"
#include "ui/event/eventMoveVisualizer.hpp"

Game::Game()
  : mConfig(parseConfig("confUI.json")),
    mPathToData(mConfig["pathToData"].asString()),
    mTileSize(1.0f),
    mHexEx(tileSize() / 2.0f),
    mHexIn(std::sqrt(std::pow(mHexEx, 2) - std::pow(mHexEx / 2.0f, 2))),
    mMode(Mode::NORMAL),
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

Game::~Game() {
  if (mEventVisualizer) {
    delete mEventVisualizer;
    mEventVisualizer = nullptr;
  }
  SDL_Quit();
}

Core& Game::core() {
  return mCore;
}

const Core& Game::core() const {
  return mCore;
}

SceneManager& Game::sceneManager() {
  return mSceneManager;
}

const SceneManager& Game::sceneManager() const {
  return mSceneManager;
}

const V2i& Game::winSize() const {
  return mWinSize;
}

Game::Mode Game::mode() const {
  return mMode;
}

Camera& Game::camera() {
  return mCamera;
}

const V2i& Game::activeTilePos() const {
  return mActiveTilePos;
}

bool Game::isRotatingCamera() const {
  return mIsRotatingCamera;
}

const V2i& Game::mousePos() const {
  return mMousePos;
}

SDL_Surface* Game::screen() {
  return mScreen;
}

float Game::tileSize() const {
  return mTileSize;
}

bool Game::done() const {
  return mDone;
}

void Game::setDone(bool done) {
  mDone = done;
}

void Game::setWinSize(const V2i& winSize) {
  mWinSize = winSize;
}

void Game::setActiveTilePos(const V2i& activeTilePos) {
  mActiveTilePos = activeTilePos;
}

void Game::setMousePos(const V2i& mousePos) {
  mMousePos = mousePos;
}

void Game::setMode(Mode mode) {
  mMode = mode;
}

void Game::setIsRotatingCamera(bool isRotatingCamera) {
  mIsRotatingCamera = isRotatingCamera;
}

void Game::setFloorTexture(int textureID) {
  mFloorTexture = textureID;
}

void Game::setScreen(SDL_Surface* screen) {
  mScreen = screen;
}

void Game::setVaWalkableMap(const VertexArray& va) {
  mVaWalkableMap = va;
}

void Game::cleanWalkableMapArray() {
  setVaWalkableMap(VertexArray());
}

void Game::rebuildWalkableMapArray() {
  setVaWalkableMap(buildWalkableArray());
}

void Game::rebuildMapArray() {
  mVaMap = buildMapArray();
}

void Game::run() {
  mainloop();
}

V2f Game::v2iToV2f(const V2i& i) const {
  assert(core().map().isInboard(i));
  V2f v(i.x() * mHexIn * 2, i.y() * mHexEx * 1.5);
  if (i.y() % 2 == 0) {
    v.setX(v.x() + mHexIn);
  }
  return v;
}

V2f Game::indexToCircleVertex(int count, int i) {
  float n = M_PI_2 + 2 * M_PI * i / count;
  return V2f(std::cos(n), std::sin(n)) * mHexEx;
}

V2f Game::indexToHexVertex(int i) {
  return indexToCircleVertex(6, i);
}

float Game::aspectRatio() const {
  float x = winSize().x();
  float y = winSize().y();
  return y / x;
}

VertexArray Game::buildMapArray() {
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

VertexArray Game::buildObstaclesArray() {
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

VertexArray Game::buildWalkableArray() {
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

void Game::drawMap() {
  mVaMap.draw();
  mVaObstacles.draw();
  mVaWalkableMap.draw();
}

void Game::recreateUnitSceneNodes() {
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

void Game::drawSelectedunitMarker() {
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

void Game::draw() {
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  camera().set();
  drawMap();
  mSceneManager.draw();
  if (mode() == Mode::SHOW_EVENT) {
    assert(mEventVisualizer);
    mEventVisualizer->draw();
  }
  if (core().isAnyUnitSelected()) {
    drawSelectedunitMarker();
  }
  SDL_GL_SwapBuffers();
}

void Game::processClickOnTile() {
  if (mode() != Mode::NORMAL) {
    return;
  }
  if (core().isUnitAt(activeTilePos())) {
    Unit& unit = core().unitAt(activeTilePos());
    if (unit.playerID() == core().currentPlayer().id) {
      core().setSelectedUnit(unit);
      core().pathfinder().fillMap(core().selectedUnit());
      setVaWalkableMap(buildWalkableArray());
    } else if (core().isAnyUnitSelected()) {
      const V2i& from = core().selectedUnit().position();
      const V2i& to = unit.position();
      bool isLosClear = core().isLosClear(from, to);
      if (isLosClear) {
        Event* e = EventAttack::generate(core(), core().selectedUnit(), unit);
        core().eventManager().addEvent(e);
      }
    }
  } else if (core().isAnyUnitSelected()) {
    Tile& tile = core().map().tile(activeTilePos());
    int actionPoints = core().selectedUnit().actionPoints();
    if (tile.cost <= actionPoints && tile.parent.value() != DirID::NONE) {
      if (core().map().tile(activeTilePos()).cost <= actionPoints) {
        Event* e = EventMove::generate(
            core(), core().selectedUnit(), activeTilePos());
        core().eventManager().addEvent(e);
      }
    }
  }
}

void Game::processSDLEvent(const SDL_MouseMotionEvent& e) {
  setMousePos(V2i(static_cast<int>(e.x), static_cast<int>(e.y)));
  if (isRotatingCamera()) {
    camera().rotateAroundZAxis(-e.xrel);
    camera().rotateAroundXAxis(-e.yrel);
  }
}

void Game::centerCameraOnSelectedUnit() {
  if (!core().isAnyUnitSelected()) {
    return;
  }
  V2f unitPos = v2iToV2f(core().selectedUnit().position());
  camera().setPos(unitPos);
}

void Game::switchActiveTileType() {
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

void Game::createNewUnitInActiveTile() {
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

void Game::processSDLEvent(const SDL_KeyboardEvent& e) {
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
  case SDLK_e: {
    Event* e = EventEndTurn::generate(core());
    core().eventManager().addEvent(e);
    break;
  }
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

void Game::processSDLEvent(const SDL_ResizeEvent& e) {
  setWinSize(V2i(e.w, e.h));
  setScreen(SDL_SetVideoMode(winSize().x(), winSize().y(),
      mBitsPerPixel, mSDLFlags));
  initOpengl();
}

void Game::processSDLEventButtonUp(const SDL_MouseButtonEvent& e) {
  if (e.button == SDL_BUTTON_RIGHT) {
    setIsRotatingCamera(false);
  }
  if (e.button == SDL_BUTTON_WHEELUP) {
    camera().zoomIn(1);
  } else if (e.button == SDL_BUTTON_WHEELDOWN) {
    camera().zoomOut(1);
  }
}

void Game::processSDLEventButtonDown(const SDL_MouseButtonEvent& e) {
  if (e.button == SDL_BUTTON_RIGHT) {
    setIsRotatingCamera(true);
  } else if (e.button == SDL_BUTTON_LEFT) {
    processClickOnTile();
  }
}

void Game::screenScenarioMainEvents() {
  core().eventManager().switchToNextEvent();
  if (mEventVisualizer) {
    delete mEventVisualizer;
  }
  mEventVisualizer = newEventVisualizer(*this, core().eventManager().currentEvent());
  assert(mEventVisualizer);
  setMode(Mode::SHOW_EVENT);
}

void Game::logic() {
  while (mode() == Mode::NORMAL && core().eventManager().unshownEventsLeft()) {
    screenScenarioMainEvents();
  }
  if (mode() == Mode::SHOW_EVENT) {
    if (mEventVisualizer->isFinished()) {
      core().eventManager().applyCurrentEvent();
      setMode(Mode::NORMAL);
      mEventVisualizer->end();
    }
  }
}

void Game::processSDLEvent(const SDL_Event& e) {
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

void Game::sdlEvents() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    processSDLEvent(e);
  }
}

VertexArray Game::buildPickingTilesArray() {
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

V2i Game::pickTile(const V2i& mousePos) {
  GLint viewport[4];
  GLubyte pixel[3];
  glGetIntegerv(GL_VIEWPORT, viewport);
  viewport[3] -= 1;
  void* pixelPointer = &pixel;
  glReadPixels(mousePos.x(), viewport[3] - mousePos.y(),
      1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixelPointer);
  return V2i(pixel[0], pixel[1]);
}

void Game::drawForPicking() {
  glLoadIdentity();
  camera().set();
  mVaPick.draw();
}

void Game::scrollMap() {
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

void Game::pickTile() {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawForPicking();
  setActiveTilePos(pickTile(mMousePos));
}

void Game::mainloop() {
  while (!done()) {
    sdlEvents();
    logic();
    scrollMap();
    pickTile();
    draw();
    // SDLDelay(1000.0f / 24.0f);
  }
}

void Game::initOpengl() {
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

void Game::initCamera() {
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

void Game::initVertexArrays() {
  mVaPick = buildPickingTilesArray();
  mVaMap = buildMapArray();
  mVaObstacles = buildObstaclesArray();
  buildUnitCirclesVertexArrays();
}

void Game::createUnitNode(const Unit& unit) {
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

void Game::buildUnitCirclesVertexArrays(){
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

void Game::loadUnitResources() {
  Json::Value resources = parseConfig("unitResources.json");
  for (const std::string& key : resources.getMemberNames()) {
    unsigned int id = core().getUnitType(key).id;
    const Json::Value& unitInfo = resources[key];
    std::string texturePath = mPathToData + unitInfo["textureName"].asString();
    std::string objModelPath = mPathToData + unitInfo["objModelName"].asString();
    mVaUnits[id] = ObjModel(objModelPath).build();
    mVaUnits[id].mTextureID = loadTexture(texturePath);
  }
}
