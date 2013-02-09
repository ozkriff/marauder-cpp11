// See LICENSE file for copyright and license details.

#ifndef CORE__TILE_HPP
#define CORE__TILE_HPP

#include "core/dir.hpp"

class Unit;

class Tile {
public: // TODO: make private
  Unit* unit;
  bool obstacle;
  // TODO: move to pathfinding
  int cost;
  Dir parent;
  Dir dir;
  int fow; // how many units see this tile
};

#endif
