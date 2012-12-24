/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include <math.h>
#include <SDL/SDL_opengl.h>
#include "core/v2i.h"
#include "core/misc.h"
#include "core/dir.h"
#include "ui_opengl/v2f.h"
#include "ui_opengl/math.h"
#include "ui_opengl/camera.h"

Camera::Camera() {
}

Camera::~Camera() {
}

void Camera::set() {
  glTranslatef(0, 0, -zoom);
  glRotatef(x_angle, -1, 0, 0);
  glRotatef(z_angle, 0, 0, 1);
  glTranslatef(-pos.x, -pos.y, 0);
}

void Camera::move(Dir d) {
  assert(!dir_is_diagonal(d));
  float speed = zoom / 20.0f;
  float in_radians = deg2rad(z_angle - dir_to_angle(d));
  pos.x += static_cast<float>(sin(in_radians)) * speed;
  pos.y += static_cast<float>(cos(in_radians)) * speed;
}
