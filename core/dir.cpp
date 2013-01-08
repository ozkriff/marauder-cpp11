// See LICENSE file for copyright and license details.

#include "core/dir.hpp"
#include <cassert>
#include <cstdlib>
#include "math.hpp"

static V2i dirToPosDiff[2][6] = {
  {
    {1, -1},
    {1, 0},
    {1, 1},
    {0, 1},
    {-1, 0},
    {0, -1},
  },
  {
    {0, -1},
    {1, 0},
    {0, 1},
    {-1, 1},
    {-1, 0},
    {-1, -1},
  }
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
  for (int i = 0; i < 6; i++) {
    if (diff == dirToPosDiff[a.y() % 2][i]) {
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
  assert(toInt() < 6);
  return (360 / 6) * toInt();
}

Dir Dir::opposite() const {
  int directionIndex = toInt() + 3;
  if (directionIndex >= 6) {
    directionIndex -= 6;
  }
  return Dir(directionIndex);
}

int Dir::diff(Dir d1) {
  int diff = abs(toInt() - d1.toInt());
  if (diff > 6 / 2) {
    diff = 6 - diff;
  }
  return diff;
}

// Get tile's neiborhood by it's index
V2i Dir::neib(const V2i& pos, Dir i) {
  assert(i.toInt() < 6);
  V2i difference = dirToPosDiff[pos.y() % 2][i.toInt()];
  return pos + difference;
}

// TODO rename
V2i Dir::getNeib(const V2i& p1, const V2i& p2, int addMe) {
  assert(addMe >= -5);
  assert(addMe <= 5);
  int d = Dir(p1, p2).toInt() + addMe;
  while (d > 6 - 1) {
    d -= 6;
  }
  while (d < 0) {
    d += 6;
  }
  assert(d >= 0 && d <= 6 - 1);
  return neib(p1, Dir(d));
}
