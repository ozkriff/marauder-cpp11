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
private:
  Core mCore;
  UIMode mUiMode;
  int mLastMoveIndex;
  int mCurrentMoveIndex;
  VertexArray mVaWalkableMap;
  VertexArray mVaFogOfWar;
  V2i mWinSize;
  V2i mMousePos;
  V2i mActiveTilePos;
  SDL_Surface* mScreen;
  bool mIsRotatingCamera;
  bool mDone;
  Camera mCamera;
  GLuint mFloorTexture;
  VertexArray mVaMap;
  VertexArray mVaObstacles;
  VertexArray mVaPick;
  ObjModel mObjUnits[static_cast<int>(UnitTypeID::COUNT)];
  VertexArray mVaUnits[static_cast<int>(UnitTypeID::COUNT)];
  GLuint mTextureUnits[static_cast<int>(UnitTypeID::COUNT)];
  TTF_Font* mFont;

public:
  Game();
  ~Game();

  int currentMoveIndex();
  int lastMoveIndex();
  Core& core();
  const V2i& winSize();
  UIMode uiMode();
  Camera& camera();
  const V2i& activeTilePos();
  bool isRotatingCamera();
  const V2i& mousePos();
  SDL_Surface* screen();
  bool done();
  TTF_Font* font();

  void setDone(bool done);
  void setWinSize(const V2i& winSize);
  void setActiveTilePos(const V2i& activeTilePos);
  void setMousePos(const V2i& mousePos);
  void setUiMode(UIMode uiMode);
  void setIsRotatingCamera(bool isRotatingCamera);
  void setFont(TTF_Font* font);
  void setFloorTexture(int textureID);
  void setLastMoveIndex(int lastMoveIndex);
  void setCurrentMoveIndex(int currentMoveIndex);
  void setScreen(SDL_Surface* screen);
  void setVaWalkableMap(const VertexArray& va);
  void setVaFogOfWar(const VertexArray& va);

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
