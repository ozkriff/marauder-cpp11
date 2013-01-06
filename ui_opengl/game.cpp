// See LICENSE file for copyright and license details.

#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cassert>
#include <ctime>
#include "ui_opengl/math.h"
#include "ui_opengl/vertex_array.h"
#include "ui_opengl/game.h"
#include "ui_opengl/gl.h"
#include "ui_opengl/widgets.h"
#include "ui_opengl/event/move.h"
#include "ui_opengl/event.h"
#include "ui_opengl/config.h"

#ifndef DATA_DIR
#error DATA_DIR undefined!
#endif
#define DATA(x) (DATA_DIR "/" x)

#define TILE_SIZE 6.0f
#define TILE_SIZE_2 (TILE_SIZE / 2.0f)

#define FOR_EACH_TILE(p) \
  for (p = V2i(0, 0); core().inboard(p); p = core().incV2i(p))

Game::Game()
  : mHexEx(TILE_SIZE_2),
    mHexIn(sqrt(pow(mHexEx, 2) - pow(mHexEx / 2, 2)))
{
  core().initLogic();
  setDone(false);
  setWinSize(V2i(WIN_WIDTH, WIN_HEIGHT));
  setActiveTilePos(V2i(0, 0));
  setMousePos(V2i(0, 0));
  setUiMode(UIMode::NORMAL);
  setIsRotatingCamera(false);
  SDL_Init(SDL_INIT_EVERYTHING);
  Uint32 flags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER;
  mScreen = SDL_SetVideoMode(winSize().x(), winSize().y(), 32, flags);
  initOpengl();
  initCamera();
  initWidgets();
  setFont(openFont(DEFAULT_FONT, 10));
  addButtons();
  setFloorTexture(loadTexture(DATA("floor.png")));
  loadUnitResources();
  initVertexArrays();
}

Game::~Game() {
  SDL_Quit();
}

int Game::currentMoveIndex() {
  return mCurrentMoveIndex;
}

int Game::lastMoveIndex() {
  return mLastMoveIndex;
}

Core& Game::core() {
  return mCore;
}

const Core& Game::core() const {
  return mCore;
}

const V2i& Game::winSize() {
  return mWinSize;
}

UIMode Game::uiMode() {
  return mUiMode;
}

Camera& Game::camera() {
  return mCamera;
}

const V2i& Game::activeTilePos() {
  return mActiveTilePos;
}

bool Game::isRotatingCamera() {
  return mIsRotatingCamera;
}

const V2i& Game::mousePos() {
  return mMousePos;
}

SDL_Surface* Game::screen() {
  return mScreen;
}

bool Game::done() {
  return mDone;
}

TTF_Font* Game::font() {
  return mFont;
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

void Game::setFont(TTF_Font* font) {
  mFont = font;
}

void Game::setFloorTexture(int textureID) {
  mFloorTexture = textureID;
}

void Game::setLastMoveIndex(int lastMoveIndex) {
  mLastMoveIndex = lastMoveIndex;
}

void Game::setCurrentMoveIndex(int currentMoveIndex) {
  mCurrentMoveIndex = currentMoveIndex;
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
  assert(core().inboard(i));
  V2f v(i.x() * mHexIn * 2, i.y() * mHexEx * 1.5);
  if (i.y() % 2 == 0) {
    v.setX(v.x() + mHexIn);
  }
  return v;
}

V2f Game::indexToHexVertex(int i) {
  return V2f(
      mHexEx * cos(M_PI_2 + 2 * M_PI * i / 6),
      mHexEx * sin(M_PI_2 + 2 * M_PI * i / 6));
}

VertexArray Game::buildMapArray() {
  VertexArray v;
  V2i p;
  FOR_EACH_TILE(p) {
    if (core().tile(p).obstacle) {
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
  FOR_EACH_TILE(p) {
    if (!core().tile(p).obstacle) {
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
  FOR_EACH_TILE(p) {
    if (core().tile(p).fow > 0) {
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
  FOR_EACH_TILE(p) {
    Tile& t = core().tile(p);
    if (t.parent.value() != DirID::NONE && t.cost < 50) {
      V2i to = Dir::neib(p, t.parent);
      if (core().inboard(to)) {
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
  glBindTexture(GL_TEXTURE_2D, mTextureUnits[u.typeID]);
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
  float n = TILE_SIZE_2 * 0.9f;
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
    if (uiMode() == UIMode::SHOW_EVENT
        && eventFilterUnit(*this, core().currentEvent(), *u))
    {
      continue;
    }
    if (core().tile(u->pos).fow == 0) {
      continue;
    }
    drawUnit(*u);
  }
}

void Game::draw() {
  glLoadIdentity();
  camera().set();
  drawMap();
  drawUnits();
  if (uiMode() == UIMode::SHOW_EVENT) {
    eventDraw(*this, core().currentEvent());
  }
  drawButtons();
  SDL_GL_SwapBuffers();
}

void Game::processSDLEvent(const SDL_MouseButtonEvent& e) {
  V2i p(static_cast<int>(e.x), static_cast<int>(e.y));
  auto b = v2iToButton(p);
  if (b) {
    if (b->callback) {
      b->callback();
    }
    return;
  }
  Unit* u = core().unitAt(activeTilePos());
  Tile& t = core().tile(activeTilePos());
  UNUSED(e);
  if (uiMode() != UIMode::NORMAL) {
    return;
  }
  if (u && u->playerID == core().currentPlayer().id) {
    core().setSelectedUnit(u);
    core().pathfinder().fillMap(*core().selectedUnit());
    mVaWalkableMap = buildWalkableArray();
  } else if (core().selectedUnit()) {
    int ap = core().selectedUnit()->actionPoints;
    if (u && u->playerID != core().currentPlayer().id) {
      if (core().selectedUnit() && u) {
        core().shoot(core().selectedUnit(), u);
      }
    } else if (t.cost <= ap && t.parent.value() != DirID::NONE) {
      generateEventMove(core(), *core().selectedUnit(), activeTilePos());
      // TODO: Move this to uiEventMove?
      mVaWalkableMap.vertices.clear();
    }
  }
}

void Game::processSDLEvent(const SDL_MouseMotionEvent& e) {
  setMousePos(V2i(static_cast<int>(e.x), static_cast<int>(e.y)));
  if (isRotatingCamera()) {
    camera().zAngle = clampAngle(camera().zAngle - e.xrel);
    camera().xAngle = clampF(camera().xAngle - e.yrel, 0, 50);
  }
}

void Game::processSDLEvent(const SDL_KeyboardEvent& e) {
  switch (e.keysym.sym) {
  case SDLK_ESCAPE:
  case SDLK_q:
    setDone(true);
    break;
  case SDLK_c: {
    if (!core().selectedUnit()) {
      return;
    }
    V2f unitPos = v2iToV2f(core().selectedUnit()->pos);
    camera().pos = unitPos;
    break;
  }
  case SDLK_t: {
    Tile& t = core().tile(activeTilePos());
    t.obstacle = !t.obstacle;
    mVaMap = buildMapArray();
    mVaObstacles = buildObstaclesArray();
    core().calculateFow();
    mVaFogOfWar = buildFowArray();
    if (core().selectedUnit()) {
      core().pathfinder().fillMap(*core().selectedUnit());
      mVaWalkableMap = buildWalkableArray();
    }
    break;
  }
  case SDLK_e:
    generateEventEndTurn(core());
    break;
  case SDLK_u:
    core().addUnit(activeTilePos(), core().currentPlayer().id);
    if (core().selectedUnit()) {
      core().pathfinder().fillMap(*core().selectedUnit());
      mVaWalkableMap = buildWalkableArray();
    }
    break;
  case SDLK_d:
    camera().zAngle = clampAngle(camera().zAngle + 15);
    break;
  case SDLK_a:
    camera().zAngle = clampAngle(camera().zAngle - 15);
    break;
  case SDLK_w:
    camera().zoom = clampF(camera().zoom - 10, 30, 200);
    break;
  case SDLK_s:
    camera().zoom = clampF(camera().zoom + 10, 30, 200);
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
  setLastMoveIndex(getLastEventIndex(*this, core().currentEvent()));
  setUiMode(UIMode::SHOW_EVENT);
  setCurrentMoveIndex(0);
  // TODO: Remove this hack
  if (core().currentEvent().t == EventTypeID::END_TURN) {
    core().applyEvent(core().currentEvent());
    setUiMode(UIMode::NORMAL);
  }
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
    setScreen(SDL_SetVideoMode(e.resize.w, e.resize.h, 32, SDL_RESIZABLE));
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
      camera().zoom = clampF(camera().zoom - 5, 30, 200);
    } else if (e.button.button == SDL_BUTTON_WHEELDOWN) {
      camera().zoom = clampF(camera().zoom + 5, 30, 200);
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
  FOR_EACH_TILE(p) {
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

bool Game::pickTile(V2i* p, const V2i* mousePos) {
  GLint viewport[4];
  GLubyte pixel[3];
  assert(p);
  glGetIntegerv(GL_VIEWPORT, viewport);
  viewport[3] -= 1;
  glReadPixels(mousePos->x(), viewport[3] - mousePos->y(),
      1, 1, GL_RGB, GL_UNSIGNED_BYTE, (void *)pixel);
  if (pixel[2] != 1) {
    return false;
  }
  p->setX(pixel[0]);
  p->setY(pixel[1]);
  return true;
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
  if (camera().pos.x() > MAP_X * TILE_SIZE) {
    camera().pos.setX(MAP_X * TILE_SIZE);
  } else if (camera().pos.x() < 0) {
    camera().pos.setX(0);
  }
  if (camera().pos.y() > MAP_Y * TILE_SIZE) {
    camera().pos.setY(MAP_Y * TILE_SIZE);
  } else if (camera().pos.y() < 0) {
    camera().pos.setY(0);
  }
}

void Game::mainloop() {
  while (!done()) {
    sdlEvents();
    logic();
    scrollMap();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawForPicking();
    pickTile(&mActiveTilePos, &mMousePos);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw();
    // SDLDelay(1000.0f / 24.0f);
  }
}

void Game::initOpengl() {
  float aspectRatio = static_cast<float>(winSize().y()) / winSize().x();
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(0.5, -0.5, -0.5 * aspectRatio,
      0.5 * aspectRatio, 1, 500);
  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_BLEND);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_COLOR_MATERIAL);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glClearColor(0, 0, 0, 1);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0.0f);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Game::initCamera() {
  camera().xAngle = 45.0f;
  camera().zAngle = 45.0f;
  camera().pos = V2f(20, 20);
  camera().zoom = 100.0f;
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
  mTextureUnits[tankID] = loadTexture(DATA("tank.png"));
  mTextureUnits[truckID] = loadTexture(DATA("truck.png"));
  mObjUnits[tankID].read(DATA("tank.obj"));
  mObjUnits[truckID].read(DATA("truck.obj"));
  mVaUnits[tankID] = mObjUnits[tankID].build();
  mVaUnits[truckID] = mObjUnits[truckID].build();
}

void Game::onTestButton() {
  printf("BUTTON CLICKED\n");
}

void Game::addButtons() {
  V2i pos = {10, 10};
  (void)addButton(font(), &pos, "[CLICK ME!]",
      nullptr); // onTestButton);
}
