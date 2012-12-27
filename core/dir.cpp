// See LICENSE file for copyright and license details.

#include "core/dir.h"
#include <cassert>
#include <cstdlib>
#include "math.h"

static V2i dirToPosDiff[] = {
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
  : mValue(DirID::NONE)
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
  assert(a.distance(b) == 1);
  V2i diff = b - a;
  for (int i = 0; i < 8; i++) {
    if (diff == dirToPosDiff[i]) {
      mValue = static_cast<DirID>(i);
      return;
    }
  }
  mValue = DirID::ERROR;
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
  return value() != DirID::N
      && value() != DirID::E
      && value() != DirID::S
      && value() != DirID::W;
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
  V2i diff = dirToPosDiff[i.toInt()];
  return pos + diff;
}

// TODO rename
V2i Dir::getNeib(const V2i& p1, const V2i& p2, int addMe) {
  assert(addMe >= -7);
  assert(addMe <= 7);
  int d = Dir(p1, p2).toInt() + addMe;
  while (d > 7) {
    d -= 8;
  }
  while (d < 0) {
    d += 8;
  }
  assert(d >= 0 && d <= 7);
  return neib(p1, Dir(d));
}
