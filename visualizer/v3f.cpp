// See LICENSE file for copyright and license details.

#include "visualizer/v3f.hpp"
#include <cassert>

V3f::V3f(float x, float y, float z)
  : mX(x),
    mY(y),
    mZ(z)
{
}

V3f::V3f(const V2f& v, float z)
  : mX(v.x()),
    mY(v.y()),
    mZ(z)
{
}

float V3f::x() const {
  return mX;
}

float V3f::y() const {
  return mY;
}

float V3f::z() const {
  return mZ;
}

void V3f::setX(float x) {
  mX = x;
}

void V3f::setY(float y) {
  mY = y;
}

void V3f::setZ(float z) {
  mZ = z;
}

V3f V3f::operator+(const V3f& b) {
  return V3f(x() + b.x(), y() + b.y(), z() + b.z());
}

V3f V3f::operator*(float b) {
  return V3f(x() * b, y() * b, z() * b);
}
