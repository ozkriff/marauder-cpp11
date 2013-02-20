// See LICENSE file for copyright and license details.

#include <cassert>
#include <cmath>
#include "ui/v2f.hpp"

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

void V2f::rotate(V2f *p, float angle) {
  assert(p);
  assert(angle >= 0);
  assert(angle <= 360);
  V2f old = *p;
  float sin = std::sin(angle);
  float cos = std::cos(angle);
  p->setX(cos * old.x() - sin * old.y());
  p->setY(sin * old.x() + cos * old.y());
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
