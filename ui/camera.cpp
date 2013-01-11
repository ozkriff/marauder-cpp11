// See LICENSE file for copyright and license details.

#include <cassert>
#include <cmath>
#include <SDL/SDL_opengl.h>
#include "core/dir.hpp"
#include "ui/math.hpp"
#include "ui/camera.hpp"

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

void Camera::move(float angle) {
  float speed = zoom / 20.0f;
  float inRadians = deg2rad(zAngle - angle);
  pos.setX(pos.x() + sin(inRadians) * speed);
  pos.setY(pos.y() + cos(inRadians) * speed);
}

void Camera::clampPosition(const V2f& max) {
  if (pos.x() > max.x()) {
    pos.setX(max.x());
  } else if (pos.x() < 0) {
    pos.setX(0);
  }
  if (pos.y() > max.y()) {
    pos.setY(max.y());
  } else if (pos.y() < 0) {
    pos.setY(0);
  }
}