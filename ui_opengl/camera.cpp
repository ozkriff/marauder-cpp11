// See LICENSE file for copyright and license details.

#include <cassert>
#include <cmath>
#include <SDL/SDL_opengl.h>
#include "core/dir.h"
#include "ui_opengl/math.h"
#include "ui_opengl/camera.h"

Camera::Camera() {
}

Camera::~Camera() {
}

void Camera::set() {
  glTranslatef(0, 0, -zoom);
  glRotatef(xAngle, -1, 0, 0);
  glRotatef(zAngle, 0, 0, 1);
  glTranslatef(-pos.x(), -pos.y(), 0);
}

void Camera::move(Dir d) {
  assert(!d.isDiagonal());
  float speed = zoom / 20.0f;
  float inRadians = deg2rad(zAngle - d.toAngle());
  pos.setX(pos.x() + sin(inRadians) * speed);
  pos.setY(pos.y() + cos(inRadians) * speed);
}
