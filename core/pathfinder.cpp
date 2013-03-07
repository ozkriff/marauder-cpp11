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
Dir Pathfinder::getParentDir(const Unit& u, const V2i& m) {
  Tile& tile = mCore.map().tile(m);
  if (tile.cost == 0) {
    return u.direction();
  } else {
    return tile.parent.opposite();
  }
}

int Pathfinder::getTileCost(const Unit& u, const V2i& t, const V2i& nb) {
  int diff = Dir(t, nb).diff(getParentDir(u, t));
  int maxAP = u.type().actionPoints - 1;
  int additionalCost[] = {0, 1, std::min<int>(maxAP, 4), maxAP};
  assert(diff >= 0 && diff <= 3);
  return 1 + additionalCost[diff];
}

void Pathfinder::processNeighbourPosition(
    const Unit& u, const V2i& p1, const V2i& p2)
{
  Tile& t1 = mCore.map().tile(p1);
  Tile& t2 = mCore.map().tile(p2);
  if (mCore.isUnitAt(p2) || t2.obstacle) {
    return;
  }
  int newcost = t1.cost + getTileCost(u, p1, p2);
  int ap = u.actionPoints();
  if (t2.cost > newcost && newcost <= ap) {
    mQueue.push(p2);
    // update neighbour tile info
    t2.cost = newcost;
    t2.dir = Dir(p2, p1);
    t2.parent = Dir(p2, p1);
  }
}

void Pathfinder::cleanMap() {
  mCore.map().forEachTile([](Tile& tile){
    tile.cost = 30000;
    tile.parent = DirID::NONE;
  });
}

void Pathfinder::tryToPushNeighbours(const Unit& u, const V2i& m) {
  assert(mCore.map().isInboard(m));
  for (int i = 0; i < 6; ++i) {
    V2i neighbourPosition = Dir::getNeighbourPosition(m, static_cast<DirID>(i));
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
  t.parent = DirID::NONE;
  t.dir = u.direction();
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
    p = Dir::getNeighbourPosition(p, mCore.map().tile(p).parent);
    assert(mCore.map().isInboard(p));
  }
  // Add start position
  path.push_back(p);
  std::reverse(path.begin(), path.end());
  return path;
}
