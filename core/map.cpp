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

bool Map::isInboard(const V2i& position) const {
  return position.x() >= 0 && position.y() >= 0
      && position.x() < size().x() && position.y() < size().y();
}

Tile& Map::tile(const V2i& position) {
  assert(isInboard(position));
  return mTiles[position.y() * mSize.x() + position.x()];
}

const Tile& Map::tile(const V2i& position) const {
  assert(isInboard(position));
  return mTiles[position.y() * mSize.x() + position.x()];
}

V2i Map::incrementPosition(const V2i& position) const {
  assert(isInboard(position));
  V2i newPosition = position;
  newPosition.setX(position.x() + 1);
  if (newPosition.x() == size().x()) {
    newPosition = V2i(0, newPosition.y() + 1);
  }
  return newPosition;
}
