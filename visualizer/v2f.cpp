// See LICENSE file for copyright and license details.

#include "visualizer/v2f.hpp"
#include <cassert>
#include <cmath>

V2f::V2f(float x, float y)
  : mX(x),
    mY(y)
{
}

V2f::V2f()
  : mX(0.0f),
    mY(0.0f)
{
}

V2f::~V2f() {
}

float V2f::x() const {
  return mX;
}

float V2f::y() const {
  return mY;
}

void V2f::setX(float x) {
  mX = x;
}

void V2f::setY(float y) {
  mY = y;
}

V2f V2f::operator+(const V2f& b) {
  return V2f(x() + b.x(), y() + b.y());
}

V2f V2f::operator-(const V2f& b) {
  return V2f(x() - b.x(), y() - b.y());
}

V2f V2f::operator+(float b) {
  return V2f(x() + b, y() + b);
}

V2f V2f::operator*(float b) {
  return V2f(x() * b, y() * b);
}

V2f V2f::operator/(float b) {
  return V2f(x() / b, y() / b);
}
