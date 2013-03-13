// See LICENSE file for copyright and license details.

#ifndef CORE__DIRECTION_HPP
#define CORE__DIRECTION_HPP

#include "core/misc.hpp"
#include "core/v2i.hpp"

enum class DirectionID {
  NorthEast,
  East,
  SouthEast,
  SouthWest,
  West,
  NorthWest,
  NONE,
  ERROR,
  COUNT
};

class Direction {
public:
  Direction();
  Direction(DirectionID value);
  Direction(int value);
  Direction(const V2i& a, const V2i& b);
  ~Direction();

  DirectionID value() const;
  int toInt() const;
  Direction opposite() const;
  int diff(Direction d1) const;

  bool operator==(const Direction& other) const;

  // Get neighbour tile in 'i' direction
  static V2i getNeighbourPosition(const V2i& position, Direction i);

private:
  DirectionID mValue;
};

#endif
