/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include <math.h>
#include <SDL/SDL_opengl.h>
#include "bool.h"
#include "list.h"
#include "misc.h"
#include "v2f.h"
#include "v2i.h"
#include "dir.h"
#include "math.h"
#include "camera.h"

void set_camera(const Camera *c) {
  assert(c);
  glTranslatef(0, 0, -(c->zoom));
  glRotatef(c->x_angle, -1, 0, 0);
  glRotatef(c->z_angle, 0, 0, 1);
  glTranslatef(c->pos.x, c->pos.y, 0);
}

void move_camera(Camera *c, Dir d) {
  float speed;
  float angle;
  float in_radians;
  assert(c);
  assert(d == D_UP || d == D_RIGHT
     || d == D_DOWN || d == D_LEFT);
  switch (d) {
    case D_UP: {
      angle = 0.0f;
      break;
    }
    case D_DOWN: {
      angle = 180.0f;
      break;
    }
    case D_LEFT: {
      angle = 90.0f;
      break;
    }
    case D_RIGHT: {
      angle = 270.0f;
      break;
    }
    default: {
      die("game.c: move_camera(): "
          "Unknown direction\n");
    }
  }
  speed = c->zoom / 20.0f;
  in_radians = deg2rad(c->z_angle + angle);
  c->pos.x -= (float)sin(in_radians) * speed;
  c->pos.y -= (float)cos(in_radians) * speed;
}
