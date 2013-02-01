// See LICENSE file for copyright and license details.

#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cassert>
#include <ctime>
#include "ui/math.hpp"
#include "ui/vertexArray.hpp"
#include "ui/game.hpp"
#include "ui/gl.hpp"
#include "ui/event/eventEndTurnVisualizer.hpp"
#include "ui/eventVisualizer.hpp"
#include "ui/event/eventAttackVisualizer.hpp"
#include "ui/event/eventMoveVisualizer.hpp"

#define DATA_DIR "/home/ozkriff/projects/my/marauder/data/"
#define DATA(x) (DATA_DIR "/" x)

Game::Game()
  : mTileSize(6.0f),
    mHexEx(tileSize() / 2.0f),
    mHexIn(std::sqrt(std::pow(mHexEx, 2) - std::pow(mHexEx / 2.0f, 2))),
    mUiMode(UIMode::NORMAL),
    mSDLFlags(SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_RESIZABLE),
    mBitsPerPixel(32),
    mWinSize(640, 480),
    mMousePos(0, 0),
    mActiveTilePos(0, 0),
    mIsRotatingCamera(false),
    mDone(false),
    mEventVisualizer(nullptr)
{
  SDL_Init(SDL_INIT_EVERYTHING);
  setScreen(SDL_SetVideoMode(winSize().x(), winSize().y(),
      mBitsPerPixel, mSDLFlags));
  initOpengl();
  setFloorTexture(loadTexture(DATA("floor.png")));
  initCamera();
  loadUnitResources();
  initVertexArrays();
}

Game::~Game() {
  SDL_Quit();
}

Core& Game::core() {
  return mCore;
}

const Core& Game::core() const {
  return mCore;
}

const V2i& Game::winSize() const {
  return mWinSize;
}

UIMode Game::uiMode() const {
  return mUiMode;
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

void Game::setUiMode(UIMode uiMode) {
  mUiMode = uiMode;
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

void Game::setVaFogOfWar(const VertexArray& va) {
  mVaFogOfWar = va;
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

V2f Game::indexToHexVertex(int i) {
  return V2f(
      mHexEx * std::cos(M_PI_2 + 2 * M_PI * i / 6),
      mHexEx * std::sin(M_PI_2 + 2 * M_PI * i / 6));
}

float Game::aspectRatio() const {
  float x = winSize().x();
  float y = winSize().y();
  return y / x;
}

VertexArray Game::buildMapArray() {
  VertexArray v;
  V2i p;
  FOR_EACH_TILE(core().map(), p) {
    if (core().map().tile(p).obstacle) {
      continue;
    }
    V2f pos = v2iToV2f(p);
    for (int i = 0; i < 6; i++) {
      appendV2f(&v.vertices, pos + indexToHexVertex(i));
      appendV2f(&v.vertices, pos + indexToHexVertex(i + 1));
      appendV2f(&v.vertices, pos);
      appendV2f(&v.textureCoordinates, V2f(0.0f, 0.0f));
      appendV2f(&v.textureCoordinates, V2f(1.0f, 0.0f));
      appendV2f(&v.textureCoordinates, V2f(0.5f, 0.5f));
    }
  }
  return v;
}

VertexArray Game::buildObstaclesArray() {
  VertexArray v;
  V2i p;
  FOR_EACH_TILE(core().map(), p) {
    if (!core().map().tile(p).obstacle) {
      continue;
    }
    V2f pos = v2iToV2f(p);
    for (int i = 0; i < 6; i++) {
      appendV2f(&v.vertices, pos + indexToHexVertex(i));
      appendV2f(&v.vertices, pos + indexToHexVertex(i + 1));
      appendV2f(&v.vertices, pos);
      appendV2f(&v.textureCoordinates, V2f(0.0f, 0.0f));
      appendV2f(&v.textureCoordinates, V2f(1.0f, 0.0f));
      appendV2f(&v.textureCoordinates, V2f(0.5f, 0.5f));
    }
  }
  return v;
}

VertexArray Game::buildFowArray() {
  VertexArray v;
  V2i p;
  FOR_EACH_TILE(core().map(), p) {
    if (core().map().tile(p).fow > 0) {
      continue;
    }
    V2f pos = v2iToV2f(p);
    float h = 0.05f;
    for (int i = 0; i < 6; i++) {
      V2f a = indexToHexVertex(i);
      V2f b = indexToHexVertex(i + 1);
      appendV3f(&v.vertices, V3f(pos.x() + a.x(), pos.y() + a.y(), h));
      appendV3f(&v.vertices, V3f(pos.x() + b.x(), pos.y() + b.y(), h));
      appendV3f(&v.vertices, V3f(pos.x(), pos.y(), h));
      appendV2f(&v.textureCoordinates, V2f(0.0f, 0.0f));
      appendV2f(&v.textureCoordinates, V2f(1.0f, 0.0f));
      appendV2f(&v.textureCoordinates, V2f(0.5f, 0.5f));
    }
  }
  return v;
}

VertexArray Game::buildWalkableArray() {
  VertexArray v;
  V2i p;
  FOR_EACH_TILE(core().map(), p) {
    Tile& t = core().map().tile(p);
    if (t.parent.value() != DirID::NONE && t.cost < 50) {
      V2i to = Dir::neib(p, t.parent);
      if (core().map().isInboard(to)) {
        V2f fromF = v2iToV2f(p);
        V2f toF = v2iToV2f(to);
        appendV3f(&v.vertices, V3f(fromF.x(), fromF.y(), 0.1f));
        appendV3f(&v.vertices, V3f(toF.x(), toF.y(), 0.1f));
      }
    }
  }
  return v;
}

void Game::drawMap() {
  glEnable(GL_TEXTURE_2D);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glBindTexture(GL_TEXTURE_2D, mFloorTexture);

  glColor3f(1.0f, 1.0f, 1.0f);
  glTexCoordPointer(2, GL_FLOAT, 0, mVaMap.textureCoordinates.data());
  glVertexPointer(2, GL_FLOAT, 0, mVaMap.vertices.data());
  glDrawArrays(GL_TRIANGLES, 0, mVaMap.vertices.size() / 2);

  glColor3f(1.0f, 0.0f, 0.0f);
  glTexCoordPointer(2, GL_FLOAT, 0, mVaObstacles.textureCoordinates.data());
  glVertexPointer(2, GL_FLOAT, 0, mVaObstacles.vertices.data());
  glDrawArrays(GL_TRIANGLES, 0, mVaObstacles.vertices.size() / 2);

  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisable(GL_TEXTURE_2D);
  glDisableClientState(GL_VERTEX_ARRAY);

  glEnableClientState(GL_VERTEX_ARRAY);
  glColor3f(0.0f, 0.3f, 1.0f);
  glVertexPointer(3, GL_FLOAT, 0, mVaWalkableMap.vertices.data());
  glDrawArrays(GL_LINES, 0, mVaWalkableMap.vertices.size() / 3);
  glDisableClientState(GL_VERTEX_ARRAY);

  glEnableClientState(GL_VERTEX_ARRAY);
  glColor4f(0.0f, 0.0f, 0.0f, 0.2f);
  glVertexPointer(3, GL_FLOAT, 0, mVaFogOfWar.vertices.data());
  glDrawArrays(GL_TRIANGLES, 0, mVaFogOfWar.vertices.size() / 3);
  glDisableClientState(GL_VERTEX_ARRAY);
}

void Game::drawUnitModel(const Unit& u) {
  glEnable(GL_TEXTURE_2D);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glBindTexture(GL_TEXTURE_2D, mUnitTextureIDs[u.typeID]);
  glColor3f(1, 1, 1);
  glTexCoordPointer(2, GL_FLOAT, 0, mVaUnits[u.typeID].textureCoordinates.data());
  glVertexPointer(3, GL_FLOAT, 0, mVaUnits[u.typeID].vertices.data());
  glDrawArrays(GL_TRIANGLES, 0, mVaUnits[u.typeID].vertices.size() / 3);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisable(GL_TEXTURE_2D);
}

void Game::drawUnitCircle(const Unit& u) {
  std::vector<float> v;
  if (u.playerID == 0) {
    glColor3f(1, 0, 0);
  } else if (u.playerID == 1) {
    glColor3f(0, 0, 1);
  } else {
    die("drawUnitCircle(): You need more colors!");
  }
  float n = (tileSize() / 2.0f) * 0.9f;
  appendV3f(&v, V3f(n, n, 0.1f));
  appendV3f(&v, V3f(n, -n, 0.1f));
  appendV3f(&v, V3f(-n, -n, 0.1f));
  appendV3f(&v, V3f(-n, n, 0.1f));
  glLineWidth(2);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, v.data());
  glDrawArrays(GL_LINE_LOOP, 0, v.size() / 3);
  glDisableClientState(GL_VERTEX_ARRAY);
  glLineWidth(1);
}

void Game::drawUnit(const Unit& u) {
  V2f f = v2iToV2f(u.pos);
  glColor3f(1, 0, 0);
  glPushMatrix();
  glTranslatef(f.x(), f.y(), 0);
  glRotatef(u.dir.toInt() * 60.0f + 120.0f, 0, 0, 1);
  drawUnitModel(u);
  drawUnitCircle(u);
  glPopMatrix();
}

void Game::drawUnits() {
  for (auto u : core().units()) {
    if (uiMode() == UIMode::SHOW_EVENT) {
      assert(mEventVisualizer);
      if (mEventVisualizer->isUnitVisible(*u)) {
        continue;
      }
    }
    if (core().map().tile(u->pos).fow > 0) {
      drawUnit(*u);
    }
  }
}

void Game::draw() {
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  camera().set();
  drawMap();
  drawUnits();
  if (uiMode() == UIMode::SHOW_EVENT) {
    assert(mEventVisualizer);
    mEventVisualizer->draw();
    if (mEventVisualizer->isFinished()) {
      core().applyEvent(core().currentEvent());
      core().setCurrentEvent(nullptr);
      setUiMode(UIMode::NORMAL);
      mEventVisualizer->end();
    }
  }
  SDL_GL_SwapBuffers();
}

void Game::processSDLEvent(const SDL_MouseButtonEvent& e) {
  UNUSED(e);
  if (uiMode() != UIMode::NORMAL) {
    return;
  }
  if (core().isUnitAt(activeTilePos())) {
    Unit& unit = core().unitAt(activeTilePos());
    if (unit.playerID == core().currentPlayer().id) {
      core().setSelectedUnit(unit);
      core().pathfinder().fillMap(core().selectedUnit());
      setVaWalkableMap(buildWalkableArray());
    } else if (core().isAnyUnitSelected()) {
      EventAttack::generate(core(), core().selectedUnit(), unit);
    }
  } else if (core().isAnyUnitSelected()) {
    Tile& tile = core().map().tile(activeTilePos());
    int actionPoints = core().selectedUnit().actionPoints;
    if (tile.cost <= actionPoints && tile.parent.value() != DirID::NONE) {
      generateEventMove(core(), core().selectedUnit(), activeTilePos());
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
  V2f unitPos = v2iToV2f(core().selectedUnit().pos);
  camera().setPos(unitPos);
}

void Game::switchActiveTileType() {
  Tile& t = core().map().tile(activeTilePos());
  t.obstacle = !t.obstacle;
  mVaMap = buildMapArray();
  mVaObstacles = buildObstaclesArray();
  core().calculateFow();
  mVaFogOfWar = buildFowArray();
  if (core().isAnyUnitSelected()) {
    core().pathfinder().fillMap(core().selectedUnit());
    mVaWalkableMap = buildWalkableArray();
  }
}

void Game::createNewUnitInActiveTile() {
  core().addUnit(activeTilePos(), core().currentPlayer().id);
  if (core().isAnyUnitSelected()) {
    core().pathfinder().fillMap(core().selectedUnit());
    mVaWalkableMap = buildWalkableArray();
  }
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
  case SDLK_e:
    generateEventEndTurn(core());
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
    printf("processKeyDownEvent(): "
        "Unknown key (%d, 0x%x) was pressed.\n",
        e.keysym.sym, e.keysym.sym);
    break;
  }
}

void Game::screenScenarioMainEvents() {
  core().setCurrentEvent(core().getNextEvent());
  if (mEventVisualizer) {
    delete mEventVisualizer;
  }
  mEventVisualizer = newEventVisualizer(*this, core().currentEvent());
  assert(mEventVisualizer);
  setUiMode(UIMode::SHOW_EVENT);
}

void Game::logic() {
  while (uiMode() == UIMode::NORMAL && core().unshownEventsLeft()) {
    screenScenarioMainEvents();
  }
}

void Game::processSDLEvent(const SDL_Event& e) {
  switch (e.type) {
  case SDL_QUIT:
    setDone(true);
    break;
  case SDL_VIDEORESIZE:
    setWinSize(V2i(e.resize.w, e.resize.h));
    setScreen(SDL_SetVideoMode(winSize().x(), winSize().y(),
	mBitsPerPixel, mSDLFlags));
    initOpengl();
    break;
  case SDL_KEYDOWN:
    processSDLEvent(e.key);
    break;
  case SDL_MOUSEMOTION:
    processSDLEvent(e.motion);
    break;
  case SDL_MOUSEBUTTONUP:
    if (e.button.button == SDL_BUTTON_RIGHT) {
      setIsRotatingCamera(false);
    }
    if (e.button.button == SDL_BUTTON_WHEELUP) {
      camera().zoomIn(5);
    } else if (e.button.button == SDL_BUTTON_WHEELDOWN) {
      camera().zoomOut(5);
    }
    break;
  case SDL_MOUSEBUTTONDOWN:
    if (e.button.button == SDL_BUTTON_RIGHT) {
      setIsRotatingCamera(true);
    } else if (e.button.button == SDL_BUTTON_LEFT) {
      processSDLEvent(e.button);
    }
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
  V2i p;
  FOR_EACH_TILE(core().map(), p) {
    V2f pos = v2iToV2f(p);
    for (int i = 0; i < 6; i++) {
      appendV2f(&v.vertices, pos + indexToHexVertex(i));
      appendV2f(&v.vertices, pos + indexToHexVertex(i + 1));
      appendV2f(&v.vertices, pos);
      appendRGB(&v.colors, p.x(), p.y(), 1);
      appendRGB(&v.colors, p.x(), p.y(), 1);
      appendRGB(&v.colors, p.x(), p.y(), 1);
    }
  }
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
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glColorPointer(3, GL_UNSIGNED_BYTE, 0, mVaPick.colors.data());
  glVertexPointer(2, GL_FLOAT, 0, mVaPick.vertices.data());
  glDrawArrays(GL_TRIANGLES, 0, mVaPick.vertices.size() / 2);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
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
  camera().setMaxZoom(200.0f);
  camera().setMinZoom(30.0f);
  camera().setZoom(100.0f);
}

void Game::initVertexArrays() {
  mVaPick = buildPickingTilesArray();
  mVaMap = buildMapArray();
  mVaObstacles = buildObstaclesArray();
  mVaFogOfWar = buildFowArray();
}

void Game::loadUnitResources() {
  int tankID = static_cast<int>(UnitTypeID::TANK);
  int truckID = static_cast<int>(UnitTypeID::TRUCK);
  mUnitTextureIDs[tankID] = loadTexture(DATA("tank.png"));
  mUnitTextureIDs[truckID] = loadTexture(DATA("truck.png"));
  mVaUnits[tankID] = ObjModel(DATA("tank.obj")).build();
  mVaUnits[truckID] = ObjModel(DATA("truck.obj")).build();
}
