/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include <math.h>
#include <SDL/SDL_opengl.h>
#include "core/bool.h"
#include "core/list.h"
#include "core/v2i.h"
#include "core/misc.h"
#include "core/dir.h"
#include "ui_opengl/v2f.h"
#include "ui_opengl/math.h"
#include "ui_opengl/camera.h"

void set_camera(const Camera *c) {
  assert(c);
  glTranslatef(0, 0, -(c->zoom));
  glRotatef(c->x_angle, -1, 0, 0);
  glRotatef(c->z_angle, 0, 0, 1);
  glTranslatef(-c->pos.x, -c->pos.y, 0);
}

void move_camera(Camera *c, Dir d) {
  float speed;
  float in_radians;
  assert(c);
  assert(!dir_is_diagonal(d));
  speed = c->zoom / 20.0f;
  in_radians = deg2rad(c->z_angle + dir_to_angle(d));
  c->pos.x += CAST(sin(in_radians), float) * speed;
  c->pos.y += CAST(cos(in_radians), float) * speed;
}
