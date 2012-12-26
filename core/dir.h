// See LICENSE file for copyright and license details.

#ifndef CORE_DIR_H
#define CORE_DIR_H

#include "core/misc.h"
#include "core/v2i.h"

enum class DirID {
  D_N,
  D_NE,
  D_E,
  D_SE,
  D_S,
  D_SW,
  D_W,
  D_NW,
  D_NONE,
  D_ERROR,
  D_COUNT
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
  float toAngle();
  bool isDiagonal() const;
  Dir opposite() const;
  int diff(Dir d1);

  static V2i neib(const V2i& pos, Dir i);

  // TODO: rename
  static V2i get_neib(const V2i& p1, const V2i& p2, int add_me);
};

#endif
