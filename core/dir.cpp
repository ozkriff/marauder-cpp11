// See LICENSE file for copyright and license details.

#include "core/dir.h"
#include <cassert>
#include <cstdlib>
#include "math.h"

static V2i dirToPosDiff[8] = {
  {1, 0},
  {1, 1},
  {0, 1},
  {-1, 1},
  {-1, 0},
  {-1, -1},
  {0, -1},
  {1, -1},
};

Dir::Dir()
  : mValue(DirID::D_NONE)
{
}

Dir::Dir(DirID value)
  : mValue(value)
{
}

Dir::Dir(int value)
  : mValue(static_cast<DirID>(value))
{
}

Dir::Dir(const V2i& a, const V2i& b) {
  assert(dist(a, b) == 1);
  int dx = b.x - a.x;
  int dy = b.y - a.y;
  for (int i = 0; i < 8; i++) {
    if (dx == dirToPosDiff[i].x && dy == dirToPosDiff[i].y) {
      mValue = static_cast<DirID>(i);
      return;
    }
  }
  mValue = DirID::D_ERROR;
  assert(false);
}

Dir::~Dir() {
}

DirID Dir::value() const {
  return mValue;
}

int Dir::toInt() const {
  return static_cast<int>(mValue);
}

float Dir::toAngle() {
  assert(toInt() < 8);
  return (360 / 8) * toInt();
}

bool Dir::isDiagonal() const {
  return value() != DirID::D_N
      && value() != DirID::D_E
      && value() != DirID::D_S
      && value() != DirID::D_W;
}

Dir Dir::opposite() const {
  int directionIndex = toInt() + 4;
  if (directionIndex >= 8) {
    directionIndex -= 8;
  }
  return Dir(directionIndex);
}

int Dir::diff(Dir d1) {
  int diff = abs(toInt() - d1.toInt());
  if (diff > 4) {
    diff = 8 - diff;
  }
  return diff;
}

// Get tile's neiborhood by it's index
V2i Dir::neib(const V2i& pos, Dir i) {
  assert(i.toInt() < 8);
  int dx = dirToPosDiff[i.toInt()].x;
  int dy = dirToPosDiff[i.toInt()].y;
  return V2i(pos.x + dx, pos.y + dy);
}

// TODO rename
V2i Dir::get_neib(const V2i& p1, const V2i& p2, int add_me) {
  assert(add_me >= -7);
  assert(add_me <= 7);
  int d = Dir(p1, p2).toInt() + add_me;
  while (d > 7) {
    d -= 8;
  }
  while (d < 0) {
    d += 8;
  }
  assert(d >= 0 && d <= 7);
  return neib(p1, Dir(d));
}
