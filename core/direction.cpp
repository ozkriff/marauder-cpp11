// See LICENSE file for copyright and license details.

#include "core/direction.hpp"
#include <cassert>
#include <cstdlib>
#include "math.hpp"

namespace {

V2i dirToPositionDiff[2][6] = {
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

} // namespace

Direction::Direction()
  : mValue(DirectionID::NONE)
{
}

Direction::Direction(DirectionID value)
  : mValue(value)
{
}

Direction::Direction(int value)
  : mValue(static_cast<DirectionID>(value))
{
}

Direction::Direction(const V2i& a, const V2i& b) {
  assert(a.distance(b) == 1);
  V2i diff = b - a;
  for (int i = 0; i < 6; ++i) {
    if (diff == dirToPositionDiff[a.y() % 2][i]) {
      mValue = static_cast<DirectionID>(i);
      return;
    }
  }
  mValue = DirectionID::ERROR;
  assert(false);
}

Direction::~Direction() {
}

DirectionID Direction::value() const {
  return mValue;
}

int Direction::toInt() const {
  return static_cast<int>(mValue);
}

Direction Direction::opposite() const {
  int directionIndex = toInt() + 6 / 2;
  if (directionIndex >= 6) {
    directionIndex -= 6;
  }
  return Direction(directionIndex);
}

int Direction::diff(Direction d1) const {
  int diff = std::abs(toInt() - d1.toInt());
  if (diff > 6 / 2) {
    diff = 6 - diff;
  }
  return diff;
}

bool Direction::operator==(const Direction& other) const {
  return value() == other.value();
}

V2i Direction::getNeighbourPosition(const V2i& position, Direction i) {
  bool isOddRow = (position.y() % 2 == 1);
  int subtableIndex = static_cast<int>(isOddRow); // 0 or 1
  int directionIndex = i.toInt();
  assert(directionIndex < 6);
  V2i difference = dirToPositionDiff[subtableIndex][directionIndex];
  return position + difference;
}
