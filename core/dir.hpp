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
private:
  DirID mValue;

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

  static V2i neib(const V2i& pos, Dir i);

  // TODO: rename
  static V2i getNeib(const V2i& p1, const V2i& p2, int addMe);
};

#endif
