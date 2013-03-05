// See LICENSE file for copyright and license details.

#ifndef CORE__MAP_HPP
#define CORE__MAP_HPP

#include "core/tile.hpp"
#include <vector>

class Map {
public:
  Map(const V2i& size);
  ~Map();

  void resize(const V2i& size);
  const V2i& size() const;

  bool isInboard(const V2i& pos) const;

  const Tile& tile(const V2i& pos) const;
  Tile& tile(const V2i& pos);

  V2i incV2i(const V2i& pos) const;

  template <class Func>
  void forEachPos(Func f) {
    for (V2i p(0, 0); isInboard(p); p = incV2i(p)) {
      f(p);
    }
  }

  template <class Func>
  void forEachTile(Func f) {
    for (Tile& tile : mTiles) {
      f(tile);
    }
  }

private:
  std::vector<Tile> mTiles;
  V2i mSize;
};

#endif
