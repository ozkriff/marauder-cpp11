// See LICENSE file for copyright and license details.

#ifndef CORE_UNIT_H
#define CORE_UNIT_H

#include "core/dir.hpp"
#include "core/v2i.hpp"

struct Unit {
  int typeID;
  int id;
  int playerID;
  Dir dir;
  int actionPoints;
  V2i pos;
};

#endif
