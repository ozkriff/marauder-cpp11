/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "bool.h"
#include "v2f.h"
#include "math.h"

void fixnum(int min, int max, int *n) {
  assert(n);
  if (*n < min) {
    *n = min;
  } else if (*n > max) {
    *n = max;
  }
}

int rnd(int min, int max) {
  assert(min < max);
  if (max != min) {
    return rand() % (max - min) + min;
  } else {
    return max;
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

void fix_angle(float *angle) {
  assert(angle);
  if (*angle < 0) {
    *angle = 0;
  } else if (*angle > 360) {
    *angle = 360;
  }
}

float dist(const V2f *a, const V2f *b) {
  double dx, dy;
  assert(a);
  assert(b);
  dx = abs(b->x - a->x);
  dy = abs(b->y - a->y);
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
  dx = old.x - o->x;
  dy = old.y - o->y;
  p->x = cs * dx - sn * dy + o->x;
  p->y = sn * dx + cs * dy + o->y;
}
