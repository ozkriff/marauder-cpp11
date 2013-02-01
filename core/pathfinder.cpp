// See LICENSE file for copyright and license details.

#include <cstdlib>
#include <cassert>
#include <algorithm>
#include "core/pathfinder.hpp"
#include "core/core.hpp"

Pathfinder::Pathfinder(Core& core)
  : mQueue(core, 1000),
    mCore(core)
{
}

// If this is first(start) tile - no parent tile
Dir Pathfinder::getParentDir(const Unit& u, const V2i& m) {
  Tile& tile = mCore.map().tile(m);
  if (tile.cost == 0) {
    return u.dir;
  } else {
    return tile.parent.opposite();
  }
}

int Pathfinder::getTileCost(const Unit& u, const V2i& t, const V2i& nb) {
  int diff = Dir(t, nb).diff(getParentDir(u, t));
  int maxAP = getUnitType(u.typeID).actionPoints - 1;
  int additionalCost[] = {0, 3, std::min<int>(maxAP, 6), maxAP};
  assert(diff >= 0 && diff <= 3);
  return 1 + additionalCost[diff];
}

// TODO: rename
// p1 - origPos, p2 - neib pos
void Pathfinder::processNeibor(const Unit& u, const V2i& p1, const V2i& p2) {
  Tile& t1 = mCore.map().tile(p1);
  Tile& t2 = mCore.map().tile(p2);
  if (mCore.isUnitAt(p2) || t2.obstacle) {
    return;
  }
  int newcost = t1.cost + getTileCost(u, p1, p2);
  int ap = u.actionPoints;
  if (t2.cost > newcost && newcost <= ap) {
    mQueue.push(p2, Dir(p2, p1), newcost, Dir(p1, p2));
  }
}

void Pathfinder::cleanMap() {
  V2i p;
  FOR_EACH_TILE(mCore.map(), p) {
    Tile& t = mCore.map().tile(p);
    t.cost = 30000;
    t.parent = DirID::NONE;
  }
}

void Pathfinder::tryToPushNeibors(const Unit& u, const V2i& m) {
  assert(mCore.map().isInboard(m));
  for (int i = 0; i < 6; i++) {
    V2i neibM = Dir::neib(m, static_cast<DirID>(i));
    if (mCore.map().isInboard(neibM)) {
      processNeibor(u, m, neibM);
    }
  }
}

void Pathfinder::fillMap(const Unit& u) {
  assert(mQueue.isEmpty());
  cleanMap();
  // Push start position
  mQueue.push(u.pos, DirID::NONE, 0, u.dir);
  while (!mQueue.isEmpty()) {
    V2i p = mQueue.pop();
    tryToPushNeibors(u, p);
  }
}

std::vector<V2i> Pathfinder::getPath(const V2i& pos) {
  V2i p = pos;
  std::vector<V2i> path;
  assert(mCore.map().isInboard(p));
  while (mCore.map().tile(p).cost != 0) {
    path.push_back(p);
    p = Dir::neib(p, mCore.map().tile(p).parent);
    assert(mCore.map().isInboard(p));
  }
  // Add start position
  path.push_back(p);
  std::reverse(path.begin(), path.end());
  return path;
}
