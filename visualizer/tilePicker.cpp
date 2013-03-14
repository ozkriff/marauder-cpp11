// See LICENSE file for copyright and license details.

#include "visualizer/tilePicker.hpp"
#include "SDL_opengl.h"
#include "core/v2i.hpp"
#include "visualizer/meshBuilders.hpp"
#include "visualizer/camera.hpp"

// public:

TilePicker::TilePicker(const Map& map, Camera& camera)
  : mCamera(camera)
{
  mPickMesh = buildPickingTilesMesh(map);
}

V2i TilePicker::pick(const V2i& mousePosition) {
  drawForPicking();
  return pickTile(mousePosition);
}

// private:

V2i TilePicker::pickTile(const V2i& mousePosition) {
  GLint viewport[4];
  GLubyte pixel[3];
  glGetIntegerv(GL_VIEWPORT, viewport);
  viewport[3] -= 1;
  void* pixelPointer = &pixel;
  glReadPixels(mousePosition.x(), viewport[3] - mousePosition.y(),
      1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixelPointer);
  return V2i(pixel[0], pixel[1]);
}

void TilePicker::drawForPicking() {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  mCamera.set();
  mPickMesh.draw();
}
