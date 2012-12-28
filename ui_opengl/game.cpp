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
  for (p = V2i(0, 0); core.inboard(p); p = core.incV2i(p))

Game::Game() {
}

Game::~Game() {
}

void Game::run() {
  init();
  mainloop();
  cleanup();
}

void Game::init() {
  core.initLogic();
  done = false;
  winSize = V2i(WIN_WIDTH, WIN_HEIGHT);
  activeTilePos = V2i(0, 0);
  mousePos = V2i(0, 0);
  uiMode = UIMode::NORMAL;
  isRotatingCamera = false;
  SDL_Init(SDL_INIT_EVERYTHING);
  screen = SDL_SetVideoMode(winSize.x(), winSize.y(),
      32, SDL_OPENGL | SDL_GL_DOUBLEBUFFER);
  initOpengl();
  initCamera();
  initWidgets();
  font = openFont(DEFAULT_FONT, 10);
  addButtons();
  loadTexture(&floorTexture, DATA("floor.png"));
  loadUnitResources();
  initVertexArrays();
}

void Game::cleanup() {
  SDL_Quit();
}

V2f Game::v2iToV2f(const V2i& i) {
  assert(core.inboard(i));
  return V2f(i.x() * TILE_SIZE, i.y() * TILE_SIZE);
}

void Game::buildMapArray(VertexArray *v) {
  V2i p;
  int i = 0; // tile's index
  v->count = MAP_X * MAP_Y * 6;
  if (v->v) {
    delete[] v->v;
    v->v = nullptr;
  }
  if (v->t) {
    delete[] v->t;
    v->t = nullptr;
  }
  v->v = (float*)new V3f[v->count];
  v->t = (float*)new V2f[v->count];
  FOR_EACH_TILE(p) {
    Tile& t = core.tile(p);
    float n = TILE_SIZE_2;
    if (t.obstacle) {
      continue;
    }
    V2f pos = v2iToV2f(p);
    setXY(v->v, 3, i, 0, pos.x() - n, pos.y() - n);
    setXY(v->v, 3, i, 1, pos.x() + n, pos.y() - n);
    setXY(v->v, 3, i, 2, pos.x() + n, pos.y() + n);
    setXY(v->t, 3, i, 0, 0, 0);
    setXY(v->t, 3, i, 1, 1, 0);
    setXY(v->t, 3, i, 2, 1, 1);
    i++;
    setXY(v->v, 3, i, 0, pos.x() - n, pos.y() - n);
    setXY(v->v, 3, i, 1, pos.x() + n, pos.y() + n);
    setXY(v->v, 3, i, 2, pos.x() - n, pos.y() + n);
    setXY(v->t, 3, i, 0, 0, 0);
    setXY(v->t, 3, i, 1, 1, 1);
    setXY(v->t, 3, i, 2, 0, 1);
    i++;
  }
}

void Game::buildObstaclesArray(VertexArray *v) {
  V2i p;
  int i = 0; // tile's index
  v->count = MAP_X * MAP_Y * 6;
  if (v->v) {
    delete[] v->v;
    v->v = nullptr;
  }
  if (v->t) {
    delete[] v->t;
    v->t = nullptr;
  }
  v->v = (float *)new V3f[v->count];
  v->t = (float *)new V2f[v->count];
  FOR_EACH_TILE(p) {
    Tile& t = core.tile(p);
    float n = TILE_SIZE_2;
    if (!t.obstacle) {
      continue;
    }
    V2f pos = v2iToV2f(p);
    setXY(v->v, 3, i, 0, pos.x() - n, pos.y() - n);
    setXY(v->v, 3, i, 1, pos.x() + n, pos.y() - n);
    setXY(v->v, 3, i, 2, pos.x() + n, pos.y() + n);
    setXY(v->t, 3, i, 0, 0, 0);
    setXY(v->t, 3, i, 1, 1, 0);
    setXY(v->t, 3, i, 2, 1, 1);
    i++;
    setXY(v->v, 3, i, 0, pos.x() - n, pos.y() - n);
    setXY(v->v, 3, i, 1, pos.x() + n, pos.y() + n);
    setXY(v->v, 3, i, 2, pos.x() - n, pos.y() + n);
    setXY(v->t, 3, i, 0, 0, 0);
    setXY(v->t, 3, i, 1, 1, 1);
    setXY(v->t, 3, i, 2, 0, 1);
    i++;
  }
}

int Game::calculateWalkableTilesCount() {
  int count = 0;
  V2i p;
  FOR_EACH_TILE(p) {
    Tile& t = core.tile(p);
    if (t.parent.value() != DirID::NONE && t.cost != 30000) {
      count++;
    }
  }
  return count;
}

int Game::calculateFoggedTilesCount() {
  int n = 0;
  V2i p;
  FOR_EACH_TILE(p) {
    if (core.tile(p).fow == 0) {
      n++;
    }
  }
  return n;
}

void Game::buildFowArray(VertexArray *v) {
  V2i p;
  int i = 0; // tile's index
  v->count = calculateFoggedTilesCount() * 6;
  if (v->v) {
    delete[] v->v;
    v->v = nullptr;
  }
  v->v = (float *)new V3f[v->count];
  FOR_EACH_TILE(p) {
    float n = TILE_SIZE_2;
    if (core.tile(p).fow > 0) {
      continue;
    }
    V2f pos = v2iToV2f(p);
    setXYZ(v->v, 3, i, 0, pos.x() - n, pos.y() - n, 0.05f);
    setXYZ(v->v, 3, i, 1, pos.x() + n, pos.y() - n, 0.05f);
    setXYZ(v->v, 3, i, 2, pos.x() + n, pos.y() + n, 0.05f);
    i++;
    setXYZ(v->v, 3, i, 0, pos.x() - n, pos.y() - n, 0.05f);
    setXYZ(v->v, 3, i, 1, pos.x() + n, pos.y() + n, 0.05f);
    setXYZ(v->v, 3, i, 2, pos.x() - n, pos.y() + n, 0.05f);
    i++;
  }
}

void Game::buildWalkableArray(VertexArray *v) {
  V2i p;
  int i = 0; // tile's index
  assert(v);
  if (v->v) {
    delete[] v->v;
    v->v = nullptr;
  }
  v->count = calculateWalkableTilesCount() * 3;
  if (v->count == 0) {
    return;
  }
  v->v = (float *)new V3f[v->count];
  FOR_EACH_TILE(p) {
    Tile& t = core.tile(p);
    if (t.parent.value() != DirID::NONE && t.cost < 50) {
      V2i p2 = Dir::neib(p, t.parent);
      if (core.inboard(p2)) {
        V2f pos1 = v2iToV2f(p);
        V2f pos2 = v2iToV2f(p2);
        setXYZ(v->v, 2, i, 0, pos1.x(), pos1.y(), 0.1f);
        setXYZ(v->v, 2, i, 1, pos2.x(), pos2.y(), 0.1f);
        i++;
      }
    }
  }
}

void Game::drawMap() {
  glEnable(GL_TEXTURE_2D);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glBindTexture(GL_TEXTURE_2D, floorTexture);

  glColor3f(1.0f, 1.0f, 1.0f);
  glTexCoordPointer(2, GL_FLOAT, 0, vaMap.t);
  glVertexPointer(2, GL_FLOAT, 0, vaMap.v);
  glDrawArrays(GL_TRIANGLES, 0, vaMap.count);

  glColor3f(1.0f, 0.0f, 0.0f);
  glTexCoordPointer(2, GL_FLOAT, 0, vaObstacles.t);
  glVertexPointer(2, GL_FLOAT, 0, vaObstacles.v);
  glDrawArrays(GL_TRIANGLES, 0, vaObstacles.count);

  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisable(GL_TEXTURE_2D);
  glDisableClientState(GL_VERTEX_ARRAY);

  glEnableClientState(GL_VERTEX_ARRAY);
  glColor3f(0.0f, 0.3f, 1.0f);
  glVertexPointer(3, GL_FLOAT, 0, vaWalkableMap.v);
  glDrawArrays(GL_LINES, 0, vaWalkableMap.count);
  glDisableClientState(GL_VERTEX_ARRAY);

  glEnableClientState(GL_VERTEX_ARRAY);
  glColor4f(0.0f, 0.0f, 0.0f, 0.2f);
  glVertexPointer(3, GL_FLOAT, 0, vaFogOfWar.v);
  glDrawArrays(GL_TRIANGLES, 0, vaFogOfWar.count);
  glDisableClientState(GL_VERTEX_ARRAY);
}

void Game::drawUnitModel(const Unit& u) {
  glEnable(GL_TEXTURE_2D);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glBindTexture(GL_TEXTURE_2D, textureUnits[u.typeID]);
  glColor3f(1, 1, 1);
  glTexCoordPointer(2, GL_FLOAT, 0, vaUnits[u.typeID].t);
  glVertexPointer(3, GL_FLOAT, 0, vaUnits[u.typeID].v);
  glDrawArrays(GL_TRIANGLES, 0, vaUnits[u.typeID].count);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisable(GL_TEXTURE_2D);
}

void Game::drawUnitCircle(const Unit& u) {
  float v[4 * 3];
  float n = TILE_SIZE_2 * 0.9f;
  if (u.playerID == 0) {
    glColor3f(1, 0, 0);
  } else if (u.playerID == 1) {
    glColor3f(0, 0, 1);
  } else {
    die("drawUnitCircle(): You need more colors!");
  }
  setXYZ(v, 2, 0, 0, +n, +n, 0.1f);
  setXYZ(v, 2, 0, 1, +n, -n, 0.1f);
  setXYZ(v, 2, 1, 0, -n, -n, 0.1f);
  setXYZ(v, 2, 1, 1, -n, +n, 0.1f);
  glLineWidth(2);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, v);
  glDrawArrays(GL_LINE_LOOP, 0, 4);
  glDisableClientState(GL_VERTEX_ARRAY);
  glLineWidth(1);
}

void Game::drawUnit(const Unit& u) {
  V2f f = v2iToV2f(u.pos);
  glColor3f(1, 0, 0);
  glPushMatrix();
  glTranslatef(f.x(), f.y(), 0);
  glRotatef((u.dir.toInt() + 4) * 45.0f, 0, 0, 1);
  drawUnitModel(u);
  drawUnitCircle(u);
  glPopMatrix();
}

void Game::drawUnits() {
  for (auto u : core.units) {
    if (uiMode == UIMode::SHOW_EVENT
        && eventFilterUnit(*this, *core.currentEvent, *u))
    {
      continue;
    }
    if (core.tile(u->pos).fow == 0) {
      continue;
    }
    drawUnit(*u);
  }
}

void Game::draw() {
  glLoadIdentity();
  camera.set();
  drawMap();
  drawUnits();
  if (uiMode == UIMode::SHOW_EVENT) {
    eventDraw(*this, *core.currentEvent);
  }
  drawButtons();
  SDL_GL_SwapBuffers();
}

void Game::processMouseButtonDownEvent(const SDL_MouseButtonEvent& e) {
  V2i p(static_cast<int>(e.x), static_cast<int>(e.y));
  auto b = v2iToButton(p);
  if (b) {
    if (b->callback) {
      b->callback();
    }
    return;
  }
  Unit* u = core.unitAt(activeTilePos);
  Tile& t = core.tile(activeTilePos);
  assert(core.currentPlayer);
  UNUSED(e);
  if (uiMode != UIMode::NORMAL) {
    return;
  }
  if (u && u->playerID == core.currentPlayer->id) {
    core.selectedUnit = u;
    core.pathfinder.fillMap(*core.selectedUnit);
    buildWalkableArray(&vaWalkableMap);
  } else if (core.selectedUnit) {
    auto type = getUnitType(core.selectedUnit->typeID);
    int ap = type.actionPoints;
    if (u && u->playerID != core.currentPlayer->id) {
      if (core.selectedUnit && u) {
        core.shoot(core.selectedUnit, u);
      }
    } else if (t.cost <= ap && t.parent.value() != DirID::NONE) {
      generateEventMove(core, *core.selectedUnit, activeTilePos);
      // TODO: Move this to uiEventMove?
      delete[] vaWalkableMap.v;
      vaWalkableMap = emptyVertexArray;
    }
  }
}

void Game::processMouseMotionEvent(const SDL_MouseMotionEvent& e) {
  mousePos = V2i(static_cast<int>(e.x), static_cast<int>(e.y));
  if (isRotatingCamera) {
    camera.zAngle -= e.xrel;
    camera.xAngle -= e.yrel;
    clampAngle(&camera.zAngle);
    clampF(&camera.xAngle, 0, 50);
  }
}

void Game::processKeyDownEvent(const SDL_KeyboardEvent& e) {
  switch (e.keysym.sym) {
  case SDLK_ESCAPE:
  case SDLK_q: {
    done = true;
    break;
  }
  case SDLK_c: {
    if (!core.selectedUnit) {
      return;
    }
    V2f unitPos = v2iToV2f(core.selectedUnit->pos);
    camera.pos = unitPos;
    break;
  }
  case SDLK_t: {
    Tile& t = core.tile(activeTilePos);
    t.obstacle = !t.obstacle;
    buildMapArray(&vaMap);
    buildObstaclesArray(&vaObstacles);
    core.calculateFow();
    buildFowArray(&vaFogOfWar);
    if (core.selectedUnit) {
      core.pathfinder.fillMap(*core.selectedUnit);
      buildWalkableArray(&vaWalkableMap);
    }
    break;
  }
  case SDLK_e: {
    generateEventEndTurn(core);
    break;
  }
  case SDLK_u: {
    core.addUnit(activeTilePos, core.currentPlayer->id);
    if (core.selectedUnit) {
      core.pathfinder.fillMap(*core.selectedUnit);
      buildWalkableArray(&vaWalkableMap);
    }
    break;
  }
  case SDLK_d: {
    camera.zAngle += 15;
    clampAngle(&camera.zAngle);
    break;
  }
  case SDLK_a: {
    camera.zAngle -= 15;
    clampAngle(&camera.zAngle);
    break;
  }
  case SDLK_w: {
    camera.zoom -= 10;
    clampF(&camera.zoom, 30, 200);
    break;
  }
  case SDLK_s: {
    camera.zoom += 10;
    clampF(&camera.zoom, 30, 200);
    break;
  }
  case SDLK_UP: {
    camera.move(DirID::N);
    break;
  }
  case SDLK_DOWN: {
    camera.move(DirID::S);
    break;
  }
  case SDLK_LEFT: {
    camera.move(DirID::W);
    break;
  }
  case SDLK_RIGHT: {
    camera.move(DirID::E);
    break;
  }
  default:
    printf("processKeyDownEvent(): "
        "Unknown key (%d, 0x%x) was pressed.\n",
        e.keysym.sym, e.keysym.sym);
    break;
  }
}

void Game::screenScenarioMainEvents() {
  core.currentEvent = core.getNextEvent();
  lastMoveIndex = getLastEventIndex(*this, *core.currentEvent);
  uiMode = UIMode::SHOW_EVENT;
  currentMoveIndex = 0;
  // TODO: Remove this hack
  if (core.currentEvent->t == EventTypeID::END_TURN) {
    core.applyEvent(*core.currentEvent);
    uiMode = UIMode::NORMAL;
  }
}

void Game::logic() {
  while (uiMode == UIMode::NORMAL && core.unshownEventsLeft()) {
    screenScenarioMainEvents();
  }
}

void Game::processSDLEvent(const SDL_Event& e) {
  switch (e.type) {
  case SDL_QUIT:
    done = true;
    break;
  case SDL_VIDEORESIZE:
    screen = SDL_SetVideoMode(e.resize.w, e.resize.h,
        32, SDL_RESIZABLE);
    break;
  case SDL_KEYDOWN:
    processKeyDownEvent(e.key);
    break;
  case SDL_MOUSEMOTION:
    processMouseMotionEvent(e.motion);
    break;
  case SDL_MOUSEBUTTONUP:
    if (e.button.button == SDL_BUTTON_RIGHT) {
      isRotatingCamera = false;
    }
    if (e.button.button == SDL_BUTTON_WHEELUP) {
      camera.zoom -= 5;
      clampF(&camera.zoom, 30, 200);
    } else if (e.button.button == SDL_BUTTON_WHEELDOWN) {
      camera.zoom += 5;
      clampF(&camera.zoom, 30, 200);
    }
    break;
  case SDL_MOUSEBUTTONDOWN:
    if (e.button.button == SDL_BUTTON_RIGHT) {
      isRotatingCamera = true;
    } else if (e.button.button == SDL_BUTTON_LEFT) {
      processMouseButtonDownEvent(e.button);
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

void Game::buildPickingTilesArray(VertexArray *va) {
  V2i p;
  int i = 0; // tile index
  assert(va);
  va->count = MAP_X * MAP_Y * 4;
  if (va->v) {
    delete[] va->v;
    va->v = nullptr;
  }
  if (va->ubC) {
    delete[] va->ubC;
    va->ubC = nullptr;
  }
  va->v = (float *)new V3f[va->count];
  va->ubC = new GLubyte[va->count * 3];
  FOR_EACH_TILE(p) {
    float n = TILE_SIZE_2;
    V2f pos = v2iToV2f(p);
    setXY(va->v, 4, i, 0, pos.x() - n, pos.y() - n);
    setXY(va->v, 4, i, 1, pos.x() + n, pos.y() - n);
    setXY(va->v, 4, i, 2, pos.x() + n, pos.y() + n);
    setXY(va->v, 4, i, 3, pos.x() - n, pos.y() + n);
    setRGBi(va->ubC, 4, i, 0, p.x(), p.y(), 1);
    setRGBi(va->ubC, 4, i, 1, p.x(), p.y(), 1);
    setRGBi(va->ubC, 4, i, 2, p.x(), p.y(), 1);
    setRGBi(va->ubC, 4, i, 3, p.x(), p.y(), 1);
    i++;
  }
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
  camera.set();
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glColorPointer(3, GL_UNSIGNED_BYTE, 0, vaPick.ubC);
  glVertexPointer(2, GL_FLOAT, 0, vaPick.v);
  glDrawArrays(GL_QUADS, 0, vaPick.count);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}

void Game::scrollMap() {
  const V2i *p = &mousePos;
  int offset = 15;
  if (p->x() < offset) {
    camera.move(DirID::W);
  } else if(p->x() > screen->w - offset) {
    camera.move(DirID::E);
  }
  if (p->y() < offset) {
    camera.move(DirID::N);
  } else if(p->y() > screen->h - offset) {
    camera.move(DirID::S);
  }
  if (camera.pos.x() > MAP_X * TILE_SIZE) {
    camera.pos.setX(MAP_X * TILE_SIZE);
  } else if (camera.pos.x() < 0) {
    camera.pos.setX(0);
  }
  if (camera.pos.y() > MAP_Y * TILE_SIZE) {
    camera.pos.setY(MAP_Y * TILE_SIZE);
  } else if (camera.pos.y() < 0) {
    camera.pos.setY(0);
  }
}

void Game::mainloop() {
  while (!done) {
    sdlEvents();
    logic();
    scrollMap();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawForPicking();
    pickTile(&activeTilePos, &mousePos);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw();
    // SDLDelay(1000.0f / 24.0f);
  }
}

void Game::initOpengl() {
  float aspectRatio = static_cast<float>(winSize.y()) / winSize.x();
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
  camera.xAngle = 45.0f;
  camera.zAngle = 45.0f;
  camera.pos = V2f(20, 20);
  camera.zoom = 100.0f;
}

void Game::initVertexArrays() {
  vaMap = emptyVertexArray;
  vaPick = emptyVertexArray;
  vaWalkableMap = emptyVertexArray;
  vaFogOfWar = emptyVertexArray;
  vaObstacles = emptyVertexArray;
  buildPickingTilesArray(&vaPick);
  buildMapArray(&vaMap);
  buildObstaclesArray(&vaObstacles);
  buildFowArray(&vaFogOfWar);
}

void Game::loadUnitResources() {
  int tankID = static_cast<int>(UnitTypeID::TANK);
  int truckID = static_cast<int>(UnitTypeID::TRUCK);
  loadTexture(&textureUnits[tankID], DATA("tank.png"));
  loadTexture(&textureUnits[truckID], DATA("truck.png"));
  objRead(&objUnits[tankID], DATA("tank.obj"));
  objRead(&objUnits[truckID], DATA("truck.obj"));
  objBuild(&vaUnits[tankID], &objUnits[tankID]);
  objBuild(&vaUnits[truckID], &objUnits[truckID]);
}

void Game::onTestButton() {
  printf("BUTTON CLICKED\n");
}

void Game::addButtons() {
  V2i pos = {10, 10};
  (void)addButton(font, &pos, "[CLICK ME!]",
      nullptr); // onTestButton);
}
