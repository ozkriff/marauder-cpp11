// See LICENSE file for copyright and license details.

#ifndef UI_OPENGL_CAMERA_H
#define UI_OPENGL_CAMERA_H

#include "ui_opengl/v2f.h"

class Camera {
public:
  float x_angle;
  float z_angle;
  V2f pos;
  float zoom;

public:
  Camera();
  ~Camera();

  void set();
  void move(Dir d);
};

#endif
