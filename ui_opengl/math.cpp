// See LICENSE file for copyright and license details.

#include <cassert>
#include <cstdlib>
#include <cmath>
#include "core/v2i.h"
#include "core/dir.h"
#include "ui_opengl/v2f.h"
#include "ui_opengl/math.h"

float getRotAngle(const V2f& a, const V2f& b) {
  V2f diff(
      static_cast<float>(pow(b.x() - a.x(), 2)),
      static_cast<float>(pow(b.y() - a.y(), 2)));
  float distance = sqrt(diff.x() + diff.y());
  float angle = rad2deg(asin((b.x() - a.x()) / distance));
  if (b.y() - a.y() > 0) {
    angle = -(180 + angle);
  }
  return angle;
}

void clampF(float *n, float min, float max) {
  assert(n);
  if (*n < min) {
    *n = min;
  } else if (*n > max) {
    *n = max;
  }
}

float deg2rad(float deg) {
  return (deg * M_PI) / 180.0f;
}

float rad2deg(float rad) {
  return (rad * 180.0f) / M_PI;
}

bool minF(float a, float b) {
  return (a > b) ? b : a;
}

bool maxF(float a, float b) {
  return (a < b) ? b : a;
}

void clampAngle(float *angle) {
  assert(angle);
  if (*angle < 0.0f) {
    *angle += 360.0f;
  } else if (*angle > 360.0f) {
    *angle -= 360.0f;
  }
}

float dist(const V2f& a, const V2f& b) {
  double dx = abs(b.x() - a.x());
  double dy = abs(b.y() - a.y());
  return sqrt(pow(dx, 2) + pow(dy, 2));
}

void rotatePointAroundPoint(
    V2f *p, float angle, const V2f *o)
{
  assert(p);
  assert(o);
  assert(angle >= 0);
  assert(angle <= 360);
  V2f old = *p;
  float sn = sin(angle);
  float cs = cos(angle);
  float dx = old.x() - o->x();
  float dy = old.y() - o->y();
  p->setX(cs * dx - sn * dy + o->x());
  p->setY(sn * dx + cs * dy + o->y());
}
