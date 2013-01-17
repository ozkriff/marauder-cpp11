// See LICENSE file for copyright and license details.

#ifndef UI_OPENGL_GAME_H
#define UI_OPENGL_GAME_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_opengl.h>
#include "core/core.hpp"
#include "ui/vertex_array.hpp"
#include "ui/v2f.hpp"
#include "ui/camera.hpp"
#include "ui/obj.hpp"

class EventVisualizer;

enum class UIMode {
  NORMAL,
  SHOW_EVENT
};

class Game {
private:
  float mTileSize;
  float mHexEx;
  float mHexIn;
  Core mCore;
  UIMode mUiMode;
  VertexArray mVaWalkableMap;
  VertexArray mVaFogOfWar;
  Uint32 mSDLFlags;
  int mBitsPerPixel;
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
  VertexArray mVaUnits[static_cast<int>(UnitTypeID::COUNT)];
  GLuint mUnitTextureIDs[static_cast<int>(UnitTypeID::COUNT)];
  TTF_Font* mFont;
  EventVisualizer* mEventVisualizer;

  void processSDLEvent(const SDL_MouseButtonEvent& e);
  void processSDLEvent(const SDL_MouseMotionEvent& e);
  void processSDLEvent(const SDL_KeyboardEvent& e);
  
  void processSDLEvent(const SDL_Event& e);
  void sdlEvents();
  void centerCameraOnSelectedUnit();
  void switchActiveTileType();
  void createNewUnitInActiveTile();
  VertexArray buildPickingTilesArray();
  V2i pickTile(const V2i& mousePos);
  void drawForPicking();
  void scrollMap();
  void pickTile();
  void mainloop();
  void initOpengl();
  void initCamera();
  void initVertexArrays();
  void loadUnitResources();
  void drawMap();
  void drawUnit(const Unit& u);
  void drawUnits();
  void draw();
  void screenScenarioMainEvents();
  void logic();
  void onTestButton();
  void addButtons();
  V2f indexToHexVertex(int i);
  float aspectRatio() const;

  float tileSize() const;
  bool done() const;
  SDL_Surface* screen();
  UIMode uiMode() const;
  TTF_Font* font();
  bool isRotatingCamera() const;
  const V2i& winSize() const;
  const V2i& activeTilePos() const;
  Camera& camera();
  const V2i& mousePos() const;
  
  void setDone(bool done);
  void setFont(TTF_Font* font);
  void setFloorTexture(int textureID);
  void setWinSize(const V2i& winSize);
  void setScreen(SDL_Surface* screen);
  void setActiveTilePos(const V2i& activeTilePos);
  void setMousePos(const V2i& mousePos);
  void setIsRotatingCamera(bool isRotatingCamera);

public:
  Game();
  ~Game();

  Core& core();
  const Core& core() const;

  void setUiMode(UIMode uiMode);
  void setVaWalkableMap(const VertexArray& va);
  void setVaFogOfWar(const VertexArray& va);

  void run();
  V2f v2iToV2f(const V2i& i) const;
  VertexArray buildMapArray();
  VertexArray buildObstaclesArray();
  VertexArray buildFowArray();
  VertexArray buildWalkableArray();
  void drawUnitModel(const Unit& u);
  void drawUnitCircle(const Unit& u);
};

#endif
