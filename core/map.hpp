// See LICENSE file for copyright and license details.

#ifndef CORE__MAP_HPP
#define CORE__MAP_HPP

#include "core/tile.hpp"
#include <vector>

#define FOR_EACH_TILE(map, p) \
  for (p = V2i(0, 0); map.isInboard(p); p = map.incV2i(p))

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

  V2i incV2i(const V2i& pos) const;
};

#endif
