// See LICENSE file for copyright and license details.

#ifndef CORE__DIR_HPP
#define CORE__DIR_HPP

#include "core/misc.hpp"
#include "core/v2i.hpp"

enum class DirID {
  NE,
  E,
  SE,
  SW,
  W,
  NW,
  NONE,
  ERROR,
  COUNT
};

class Dir {
public:
  Dir();
  Dir(DirID value);
  Dir(int value);
  Dir(const V2i& a, const V2i& b);
  ~Dir();

  DirID value() const;
  int toInt() const;
  float toAngle() const;
  Dir opposite() const;
  int diff(Dir d1) const;

  // Get neighbour tile in 'i' direction
  static V2i getNeighbourPos(const V2i& pos, Dir i);

private:
  DirID mValue;
};

#endif
