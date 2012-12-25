/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "core/v2i.h"
#include "core/dir.h"
#include "ui_opengl/v2f.h"
#include "ui_opengl/math.h"

float get_rot_angle(const V2f *a, const V2f *b) {
  float distance, angle;
  V2f diff_2;
  assert(a);
  assert(b);
  diff_2.setX((float)pow(b->x() - a->x(), 2));
  diff_2.setY((float)pow(b->y() - a->y(), 2));
  distance = (float)sqrt(diff_2.x() + diff_2.y());
  angle = rad2deg((float)asin((b->x() - a->x()) / distance));
  if (b->y() - a->y() > 0) {
    angle = -(180 + angle);
  }
  return angle;
}

void clamp_f(float *n, float min, float max) {
  assert(n);
  if (*n < min) {
    *n = min;
  } else if (*n > max) {
    *n = max;
  }
}

float deg2rad(float deg) {
  return (deg * (float)M_PI) / 180.0f;
}

float rad2deg(float rad) {
  return (rad * 180.0f) / (float)M_PI;
}

bool min_f(float a, float b) {
  return (a > b) ? b : a;
}

bool max_f(float a, float b) {
  return (a < b) ? b : a;
}

void clamp_angle(float *angle) {
  assert(angle);
  if (*angle < 0.0f) {
    *angle += 360.0f;
  } else if (*angle > 360.0f) {
    *angle -= 360.0f;
  }
}

float dist(const V2f *a, const V2f *b) {
  double dx, dy;
  assert(a);
  assert(b);
  dx = abs(b->x() - a->x());
  dy = abs(b->y() - a->y());
  return (float)sqrt(pow(dx, 2) + pow(dy, 2));
}

void rotate_point_around_point(
    V2f *p, float angle, const V2f *o)
{
  V2f old;
  float sn, cs, dx, dy;
  assert(p);
  assert(o);
  assert(angle >= 0);
  assert(angle <= 360);
  old = *p;
  sn = sin(angle);
  cs = cos(angle);
  dx = old.x() - o->x();
  dy = old.y() - o->y();
  p->setX(cs * dx - sn * dy + o->x());
  p->setY(sn * dx + cs * dy + o->y());
}

float dir_to_angle(Dir d) {
  assert((int)d < 8);
  return (360 / 8) * (int)d;
}
