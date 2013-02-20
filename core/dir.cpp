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

float Dir::toAngle() const {
  assert(toInt() < 6);
  return (360 / 6) * toInt() + 30;
}

Dir Dir::opposite() const {
  int directionIndex = toInt() + 3;
  if (directionIndex >= 6) {
    directionIndex -= 6;
  }
  return Dir(directionIndex);
}

int Dir::diff(Dir d1) const {
  int diff = std::abs(toInt() - d1.toInt());
  if (diff > 6 / 2) {
    diff = 6 - diff;
  }
  return diff;
}

V2i Dir::neib(const V2i& pos, Dir i) {
  assert(i.toInt() < 6);
  V2i difference = dirToPosDiff[pos.y() % 2][i.toInt()];
  return pos + difference;
}
