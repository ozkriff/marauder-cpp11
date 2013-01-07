// See LICENSE file for copyright and license details.

#ifndef CORE_MAP_H
#define CORE_MAP_H

#include "core/tile.h"
#include <vector>

class Map {
  std::vector<Tile> mTiles;
  V2i mSize;

public:
  Map(const V2i& size);
  ~Map();

  void resize(const V2i& size);
  const V2i& size() const;

  bool isInboard(const V2i& pos) const;

  const Tile& tile(const V2i& pos) const;
  Tile& tile(const V2i& pos);
};

#endif
