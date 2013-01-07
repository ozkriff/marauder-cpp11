// See LICENSE file for copyright and license details.

#ifndef CORE_TILE_H
#define CORE_TILE_H

#include "core/dir.h"

struct Unit;

struct Tile {
  Unit* unit;
  bool obstacle;
  // TODO: move to pathfinding
  int cost;
  Dir parent;
  Dir dir;
  int fow; // how many units see this tile
};

#endif
