// See LICENSE file for copyright and license details.

#ifndef VISUALIZER__TILE_PICKER_HPP
#define VISUALIZER__TILE_PICKER_HPP

#include "visualizer/mesh.hpp"

class Map;
class Camera;
class V2i;

class TilePicker {
public:
  TilePicker(const Map& map, Camera& camera);

  V2i pick(const V2i& mousePosition);

private:
  Camera& mCamera;
  Mesh mPickMesh;

  V2i pickTile(const V2i& mousePosition);
  void drawForPicking();
};

#endif
