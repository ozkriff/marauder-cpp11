// See LICENSE file for copyright and license details.

#include "core/dir.hpp"
#include <cassert>
#include <cstdlib>
#include "math.hpp"

namespace {

V2i dirToPositionDiff[8] = {
  {1, 0},
  {1, 1},
  {0, 1},
  {-1, 1},
  {-1, 0},
  {-1, -1},
  {0, -1},
  {1, -1}
};

} // namespace

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
  for (int i = 0; i < 8; ++i) {
    if (diff == dirToPositionDiff[i]) {
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

bool Dir::isDeiagonal() const {
  return value() != DirID::N
      && value() != DirID::E
      && value() != DirID::S
      && value() != DirID::W;
}

Dir Dir::opposite() const {
  int directionIndex = toInt() + 8 / 2;
  if (directionIndex >= 8) {
    directionIndex -= 8;
  }
  return Dir(directionIndex);
}

int Dir::diff(Dir d1) const {
  int diff = std::abs(toInt() - d1.toInt());
  if (diff > 8 / 2) {
    diff = 8 - diff;
  }
  return diff;
}

bool Dir::operator==(const Dir& other) const {
  return value() == other.value();
}

V2i Dir::getNeighbourPosition(const V2i& position, Dir i) {
  assert(i.toInt() < 8);
  V2i difference = dirToPositionDiff[i.toInt()];
  return position + difference;
}
