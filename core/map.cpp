#include "core/map.hpp"
#include <cassert>

Map::Map(const V2i& size)
  : mTiles(size.x() * size.y()),
    mSize(size)
{
}

Map::~Map() {
}

void Map::resize(const V2i& size) {
  mTiles.reserve(size.x() * size.y());
}

const V2i& Map::size() const {
  return mSize;
}

bool Map::isInboard(const V2i& pos) const {
  return pos.x() >= 0 && pos.y() >= 0
      && pos.x() < size().x() && pos.y() < size().y();
}

Tile& Map::tile(const V2i& pos) {
  assert(isInboard(pos));
  return mTiles[pos.y() * mSize.x() + pos.x()];
}

const Tile& Map::tile(const V2i& pos) const {
  assert(isInboard(pos));
  return mTiles[pos.y() * mSize.x() + pos.x()];
}

V2i Map::incV2i(const V2i& pos) const {
  assert(isInboard(pos));
  V2i newPos = pos;
  newPos.setX(pos.x() + 1);
  if (newPos.x() == size().x()) {
    newPos = V2i(0, newPos.y() + 1);
  }
  return newPos;
}
