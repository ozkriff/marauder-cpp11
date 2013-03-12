// See LICENSE file for copyright and license details.

#include "core/direction.hpp"
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
  for (int i = 0; i < 8; ++i) {
    if (diff == dirToPositionDiff[i]) {
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

bool Direction::isDeiagonal() const {
  return value() != DirectionID::N
      && value() != DirectionID::E
      && value() != DirectionID::S
      && value() != DirectionID::W;
}

Direction Direction::opposite() const {
  int directionIndex = toInt() + 8 / 2;
  if (directionIndex >= 8) {
    directionIndex -= 8;
  }
  return Direction(directionIndex);
}

int Direction::diff(Direction d1) const {
  int diff = std::abs(toInt() - d1.toInt());
  if (diff > 8 / 2) {
    diff = 8 - diff;
  }
  return diff;
}

bool Direction::operator==(const Direction& other) const {
  return value() == other.value();
}

V2i Direction::getNeighbourPosition(const V2i& position, Direction i) {
  assert(i.toInt() < 8);
  V2i difference = dirToPositionDiff[i.toInt()];
  return position + difference;
}
