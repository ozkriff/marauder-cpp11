// See LICENSE file for copyright and license details.

#ifndef UI_OPENGL_GAME_H
#define UI_OPENGL_GAME_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_opengl.h>
#include "core/core.h"
#include "ui_opengl/vertex_array.h"
#include "ui_opengl/v2f.h"
#include "ui_opengl/camera.h"
#include "ui_opengl/obj.h"

enum class UIMode {
  NORMAL,
  SHOW_EVENT
};

class Game {
public:
  Core core;
  UIMode uiMode;
  int lastMoveIndex;
  int currentMoveIndex;
  VertexArray vaWalkableMap;
  VertexArray vaFogOfWar;

  V2i winSize;
  V2i mousePos;
  V2i activeTilePos;
  SDL_Surface* screen;
  bool isRotatingCamera;
  bool done;
  Camera camera;
  GLuint floorTexture;
  VertexArray vaMap;
  VertexArray vaObstacles;
  VertexArray vaPick;
  ObjModel objUnits[static_cast<int>(UnitTypeID::COUNT)];
  VertexArray vaUnits[static_cast<int>(UnitTypeID::COUNT)];
  GLuint textureUnits[static_cast<int>(UnitTypeID::COUNT)];
  TTF_Font* font;

public:
  Game();
  ~Game();
  void run();
  void init();
  void cleanup();
  V2f v2iToV2f(const V2i& i);
  VertexArray buildMapArray();
  VertexArray buildObstaclesArray();
  VertexArray buildFowArray();
  VertexArray buildWalkableArray();
  void drawMap();
  void drawUnitModel(const Unit& u);
  void drawUnitCircle(const Unit& u);
  void drawUnit(const Unit& u);
  void drawUnits();
  void draw();
  void processSDLEvent(const SDL_MouseButtonEvent& e);
  void processSDLEvent(const SDL_MouseMotionEvent& e);
  void processSDLEvent(const SDL_KeyboardEvent& e);
  void screenScenarioMainEvents();
  void logic();
  void processSDLEvent(const SDL_Event& e);
  void sdlEvents();
  VertexArray buildPickingTilesArray();
  bool pickTile(V2i* p, const V2i* mousePos);
  void drawForPicking();
  void scrollMap();
  void mainloop();
  void initOpengl();
  void initCamera();
  void initVertexArrays();
  void loadUnitResources();
  void onTestButton();
  void addButtons();
};

#endif
