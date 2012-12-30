// See LICENSE file for copyright and license details.

#include <cassert>
#include <cmath>
#include "ui_opengl/v2f.h"

V2f::V2f(float x, float y)
  : mX(x),
    mY(y)
{
}

V2f::V2f(int x, int y)
  : mX(static_cast<float>(x)),
    mY(static_cast<float>(y))
{
}

V2f::V2f()
  : mX(0.0f),
    mY(0.0f)
{
}

V2f::~V2f() {
}

void V2f::rotate(V2f *p, float angle) {
  V2f old;
  float sn, cs;
  assert(p);
  assert(angle >= 0);
  assert(angle <= 360);
  old = *p;
  sn = sin(angle);
  cs = cos(angle);
  p->setX(cs * old.x() - sn * old.y());
  p->setY(sn * old.x() + cs * old.y());
}

V2f& V2f::operator+(const V2f& b) {
  mX += b.x();
  mY += b.y();
  return *this;
}
