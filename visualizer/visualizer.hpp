// See LICENSE file for copyright and license details.

#ifndef VISUALIZER__VISUALIZER_HPP
#define VISUALIZER__VISUALIZER_HPP

#include "SDL.h"
#include "SDL_opengl.h"
#include "json/json.h"
#include "core/core.hpp"
#include "visualizer/vertexArray.hpp"
#include "visualizer/v2f.hpp"
#include "visualizer/camera.hpp"
#include "visualizer/objModel.hpp"
#include "visualizer/sceneManager.hpp"

class EventVisualizer;

class Visualizer {
public:
  enum class Mode {
    NORMAL,
    SHOW_EVENT
  };

  Visualizer();
  ~Visualizer();

  Core& core();
  const Core& core() const;

  SceneManager& sceneManager();
  const SceneManager& sceneManager() const;

  void cleanWalkableMapArray();
  void rebuildWalkableMapArray();
  void rebuildMapArray();

  void run();
  V2f v2iToV2f(const V2i& i) const;

  void recreateUnitSceneNodes();

private:
  Json::Value mConfig;
  std::string mPathToData;
  float mTileSize;
  float mHexEx;
  float mHexIn;
  Core mCore;
  Mode mMode;
  VertexArray mVaWalkableMap;
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
  std::vector<VertexArray> mVaUnitCircles;
  std::map<int, VertexArray> mVaUnits;
  EventVisualizer* mEventVisualizer;
  SceneManager mSceneManager;

  void processSDLEvent(const SDL_MouseMotionEvent& e);
  void processSDLEvent(const SDL_KeyboardEvent& e);
  void processSDLEvent(const SDL_ResizeEvent& e);
  void processSDLEventButtonUp(const SDL_MouseButtonEvent& e);
  void processSDLEventButtonDown(const SDL_MouseButtonEvent& e);

  void processSDLEvent(const SDL_Event& e);
  void processClickOnTile();
  void sdlEvents();
  void centerCameraOnSelectedUnit();
  void switchActiveTileType();
  void createNewUnitInActiveTile();
  VertexArray buildPickingTilesArray();
  VertexArray buildMapArray();
  VertexArray buildObstaclesArray();
  VertexArray buildWalkableArray();
  V2i pickTile(const V2i& mousePos);
  void drawForPicking();
  void scrollMap();
  void pickTile();
  void mainloop();
  void initOpengl();
  void initCamera();
  void initVertexArrays();
  void createUnitNode(const Unit& unit);
  void buildUnitCirclesVertexArrays();
  void loadUnitResources();
  void drawMap();
  void drawSelectedunitMarker();
  void draw();
  void screenScenarioMainEvents();
  void logic();
  V2f indexToCircleVertex(int count, int i);
  V2f indexToHexVertex(int i);
  float aspectRatio() const;

  float tileSize() const;
  bool done() const;
  SDL_Surface* screen();
  Mode mode() const;
  bool isRotatingCamera() const;
  const V2i& winSize() const;
  const V2i& activeTilePos() const;
  Camera& camera();
  const V2i& mousePos() const;

  void setVaWalkableMap(const VertexArray& va);
  void setMode(Mode mode);
  void setDone(bool done);
  void setFloorTexture(int textureID);
  void setWinSize(const V2i& winSize);
  void setScreen(SDL_Surface* screen);
  void setActiveTilePos(const V2i& activeTilePos);
  void setMousePos(const V2i& mousePos);
  void setIsRotatingCamera(bool isRotatingCamera);
};

#endif
