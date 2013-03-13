// See LICENSE file for copyright and license details.

#include "visualizer/math.hpp"
#include <cassert>
#include <cstdlib>
#include <cmath>
#include "core/v2i.hpp"
#include "core/direction.hpp"

float getRotAngle(const V2f& a, const V2f& b) {
  V2f diff(std::pow(b.x() - a.x(), 2), std::pow(b.y() - a.y(), 2));
  float distance = std::sqrt(diff.x() + diff.y());
  float angle = rad2deg(std::asin((b.x() - a.x()) / distance));
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
  double dx = std::abs(b.x() - a.x());
  double dy = std::abs(b.y() - a.y());
  return std::sqrt(std::pow(dx, 2) + std::pow(dy, 2));
}

float dirToAngle(const Direction& direction) {
  return (360.0f / 8.0f) * direction.toInt() + 90; // TODO: ?!
}

V2f v2iToV2f(const V2i& i) {
  // assert(core().map().isInboard(i)); // sorry, not here
  return V2f(i.x(), i.y());
}
