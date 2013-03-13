// See LICENSE file for copyright and license details.

#ifndef VISUALIZER__VISUALIZER_HPP
#define VISUALIZER__VISUALIZER_HPP

#include "SDL.h"
#include "SDL_opengl.h"
#include "json/json.h"
#include "core/core.hpp"
#include "visualizer/mesh.hpp"
#include "visualizer/v2f.hpp"
#include "visualizer/camera.hpp"
#include "visualizer/objModel.hpp"
#include "visualizer/sceneManager.hpp"

class EventVisualizer;
class TilePicker;

class Visualizer {
public:
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

  V2f indexToCircleVertex(int count, int i) const;

  void createUnitSceneNodes();

private:
  enum class Mode {
    Normal,
    ShowEvent
  };

  Core& mCore;
  Json::Value mConfig;
  std::string mPathToData;
  Mode mMode;
  Mesh mWalkableTilesMesh;
  Uint32 mSDLFlags;
  int mBitsPerPixel;
  V2i mWinSize;
  V2i mMousePosition;
  V2i mActiveTilePosition;
  SDL_Surface* mScreen;
  bool mIsRotatingCamera;
  bool mDone;
  Camera mCamera;
  GLuint mFloorTexture;
  Mesh mTilesMesh;
  Mesh mObstaclesMesh;
  std::vector<Mesh> mUnitCircleMeshes;
  std::map<int, Mesh> mUnitMeshes;
  EventVisualizer* mCurrentEventVisualizer;
  TilePicker* mTilePicker;

  // scene manager for each local player
  std::map<PlayerID, SceneManager> mSceneManagers;

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
  void createSceneManagerForEachPlayer();
  void scrollMap();
  void updateActiveTilePosition();
  void mainloop();
  void initOpengl();
  void initCamera();
  void initMeshs();
  void createUnitNode(const Unit& unit);
  void buildUnitCircles();
  void loadUnitResources();
  void drawMap();
  void drawSelectedunitMarker();
  void draw();
  void logic();
  float aspectRatio() const;

  Camera& camera();
  EventVisualizer& currentEventVisualizer();
};

#endif
