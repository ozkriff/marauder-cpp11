// See LICENSE file for copyright and license details.

#include <cassert>
#include "core/v2i.h"

V2i::V2i(int x, int y)
  : x(x),
    y(y)
{
}

V2i::V2i()
  : x(0),
    y(0)
{
}

V2i::~V2i() {
}

bool V2i::operator==(const V2i& b) const {
  return x == b.x && y == b.y;
}

V2i V2i::operator-(const V2i& b) const {
  return V2i(x - b.x, y - b.y);
}

V2i V2i::operator+(const V2i& b) const {
  return V2i(x + b.x, y + b.y);
}
