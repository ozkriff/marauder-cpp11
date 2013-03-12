// See LICENSE file for copyright and license details.

#include <cstdlib>
#include <cassert>
#include <algorithm>
#include "core/pathfinder.hpp"
#include "core/core.hpp"

Pathfinder::Pathfinder(Core& core)
  : mCore(core)
{
}

// If this is first(start) tile - no parent tile
Direction Pathfinder::getParentDirection(const Unit& u, const V2i& m) {
  Tile& tile = mCore.map().tile(m);
  if (tile.cost == 0) {
    return u.direction();
  } else {
    return tile.parent.opposite();
  }
}

int Pathfinder::getTileCost(const Unit& u, const V2i& t, const V2i& nb) {
  int diff = Direction(t, nb).diff(getParentDirection(u, t));
  int maxAP = u.type().actionPoints - 1;
  int additionalCost[] = {3, 4, 5, maxAP, maxAP};
  assert(diff >= 0 && diff <= 4);
  return 1 + additionalCost[diff];
}

void Pathfinder::processNeighbourPosition(
    const Unit& u, const V2i& originalPosition, const V2i& neighbourPosition)
{
  Tile& t1 = mCore.map().tile(originalPosition);
  Tile& t2 = mCore.map().tile(neighbourPosition);
  if (mCore.isUnitAt(neighbourPosition) || t2.obstacle) {
    return;
  }
  int newcost = t1.cost + getTileCost(u, originalPosition, neighbourPosition);
  int ap = u.actionPoints();
  if (t2.cost > newcost && newcost <= ap) {
    mQueue.push(neighbourPosition);
    // update neighbour tile info
    t2.cost = newcost;
    t2.direction = Direction(neighbourPosition, originalPosition);
    t2.parent = Direction(neighbourPosition, originalPosition);
  }
}

void Pathfinder::cleanMap() {
  mCore.map().forEachTile([](Tile& tile){
    tile.cost = 30000;
    tile.parent = DirectionID::NONE;
  });
}

void Pathfinder::tryToPushNeighbours(const Unit& u, const V2i& m) {
  assert(mCore.map().isInboard(m));
  for (int i = 0; i < 8; ++i) {
    V2i neighbourPosition = Direction::getNeighbourPosition(m, static_cast<DirectionID>(i));
    if (mCore.map().isInboard(neighbourPosition)) {
      processNeighbourPosition(u, m, neighbourPosition);
    }
  }
}

void Pathfinder::fillMap(const Unit& u) {
  assert(mQueue.empty());
  cleanMap();
  // Push start position
  mQueue.push(u.position());
  Tile& t = mCore.map().tile(u.position());
  t.cost = 0;
  t.parent = DirectionID::NONE;
  t.direction = u.direction();
  while (!mQueue.empty()) {
    V2i p = mQueue.front();
    mQueue.pop();
    tryToPushNeighbours(u, p);
  }
}

std::vector<V2i> Pathfinder::getPath(const V2i& position) const {
  V2i p = position;
  std::vector<V2i> path;
  assert(mCore.map().isInboard(p));
  while (mCore.map().tile(p).cost != 0) {
    path.push_back(p);
    p = Direction::getNeighbourPosition(p, mCore.map().tile(p).parent);
    assert(mCore.map().isInboard(p));
  }
  // Add start position
  path.push_back(p);
  std::reverse(path.begin(), path.end());
  return path;
}
