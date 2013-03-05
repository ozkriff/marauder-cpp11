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
    Normal,
    ShowEvent
  };

  Visualizer(Core& core);
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
  Core& mCore;
  Json::Value mConfig;
  std::string mPathToData;
  float mTileSize;
  float mHexEx;
  float mHexIn;
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
  EventVisualizer* mCurrentEventVisualizer;
  SceneManager mSceneManager;

  void processSDLEvent(const SDL_MouseMotionEvent& e);
  void processSDLEvent(const SDL_KeyboardEvent& e);
  void processSDLEvent(const SDL_ResizeEvent& e);
  void processSDLEventButtonUp(const SDL_MouseButtonEvent& e);
  void processSDLEventButtonDown(const SDL_MouseButtonEvent& e);

  void processSDLEvent(const SDL_Event& e);
  void processClickOnFriendlyUnit(Unit& unit);
  void processClickOnEnemyUnit(Unit& unit);
  void processClickOnUnit(Unit& unit);
  void processClickOnEmptyTile(Tile& tile);
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
  Camera& camera();
};

#endif
