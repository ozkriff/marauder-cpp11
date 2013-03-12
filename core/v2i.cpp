// See LICENSE file for copyright and license details.

#include "core/v2i.hpp"
#include <cassert>
#include <cstdlib>
#include <cmath>

V2i::V2i(int x, int y)
  : mX(x),
    mY(y)
{
}

V2i::V2i()
  : mX(0),
    mY(0)
{
}

V2i::~V2i() {
}

int V2i::x() const {
  return mX;
}

int V2i::y() const {
  return mY;
}

void V2i::setX(int x) {
  mX = x;
}

void V2i::setY(int y) {
  mY = y;
}

int V2i::distance(const V2i &b) const {
  int dx = abs(b.x() - x());
  int dy = abs(b.y() - y());
  return sqrt(pow(dx, 2) + pow(dy, 2));
}

bool V2i::operator==(const V2i& b) const {
  return x() == b.x() && y() == b.y();
}

V2i V2i::operator-(const V2i& b) const {
  return V2i(x() - b.x(), y() - b.y());
}

V2i V2i::operator-(int b) const {
  return V2i(x() - b, y() - b);
}

V2i V2i::operator+(const V2i& b) const {
  return V2i(x() + b.x(), y() + b.y());
}

int V2i::distance(const V2i& a, const V2i &b) {
  return a.distance(b);
}
