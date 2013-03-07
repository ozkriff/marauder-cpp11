// See LICENSE file for copyright and license details.

#ifndef VISUALIZER__TILE_PICKER_HPP
#define VISUALIZER__TILE_PICKER_HPP

#include "visualizer/vertexArray.hpp"

class Visualizer;
class Camera;
class V2i;

class TilePicker {
public:
  TilePicker(Visualizer& visualizer, Camera& camera);

  V2i pick(const V2i& mousePosition);

private:
  Camera& mCamera;
  VertexArray mVaPick;

  V2i pickTile(const V2i& mousePosition);
  void drawForPicking();
};

#endif
