// See LICENSE file for copyright and license details.

#include <cassert>
#include <cstdlib>
#include <cmath>
#include "core/v2i.hpp"
#include "core/dir.hpp"
#include "ui/v2f.hpp"
#include "ui/math.hpp"

float getRotAngle(const V2f& a, const V2f& b) {
  V2f diff(pow(b.x() - a.x(), 2), pow(b.y() - a.y(), 2));
  float distance = sqrt(diff.x() + diff.y());
  float angle = rad2deg(asin((b.x() - a.x()) / distance));
  if (b.y() - a.y() > 0) {
    angle = -(180 + angle);
  }
  return angle;
}

float clampF(float n, float min, float max) {
  if (n < min) {
    return min;
  } else if (n > max) {
    return max;
  } else {
    return n;
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

float clampAngle(float angle) {
  while (angle < 0.0f) {
    angle += 360.0f;
  }
  while (angle > 360.0f) {
    angle -= 360.0f;
  }
  return angle;
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
