// See LICENSE file for copyright and license details.

#include <cstdlib>
#include <cassert>
#include <algorithm>
#include "core/path.h"
#include "core/core.h"

PathQueue::PathQueue(Core& core, int size)
  : core(core),
    mTailNodeIndex(0),
    mHeadNodeIndex(0),
    mNodes(size)
{
  assert(size > 0);
}

PathQueue::~PathQueue() {
}

bool PathQueue::isEmpty() const {
  return (mHeadNodeIndex == mTailNodeIndex);
}

void PathQueue::push(const V2i& m, Dir parent, int newcost, Dir dir) {
  Tile& t = core.tile(m);
  t.cost = newcost;
  t.parent = parent;
  t.dir = dir;
  mNodes[mTailNodeIndex] = m;
  mTailNodeIndex++;
  if (mTailNodeIndex == mNodes.size()) {
    mTailNodeIndex = 0;
  }
}

V2i PathQueue::pop() {
  assert(mHeadNodeIndex != mTailNodeIndex);
  V2i m = mNodes[mHeadNodeIndex];
  mHeadNodeIndex++;
  if (mHeadNodeIndex == mNodes.size()) {
    mHeadNodeIndex = 0;
  }
  return m;
}

Pathfinder::Pathfinder(Core& core)
  : q(core, 1000),
    core(core)
{
}

// If this is first(start) tile - no parent tile
Dir Pathfinder::getParentDir(const Unit& u, const V2i& m) {
  Tile& tile = core.tile(m);
  if (tile.cost == 0) {
    return u.dir;
  } else {
    return tile.parent.opposite();
  }
}

int Pathfinder::getTileCost(const Unit& u, const V2i& t, const V2i& nb) {
  int cost = 1;
  int dx = abs(t.x() - nb.x());
  int dy = abs(t.y() - nb.y());
  assert(dx <= 1);
  assert(dy <= 1);
  if (dx != 0) {
    cost++;
  }
  if (dy != 0) {
    cost++;
  }
  Dir d(t, nb);
  Dir d2 = getParentDir(u, t);
  int dDiff = d.diff(d2);
  int additionalCost[] = {0, 5, 20, 90};
  // assert(dDiff >= 0 && dDiff <= 2);
  // return cost + additionalCost[dDiff];
  return 2 + additionalCost[dDiff];
}

// TODO: rename
// p1 - origPos, p2 - neib pos
void Pathfinder::processNeibor(const Unit& u, const V2i& p1, const V2i& p2) {
  Tile& t1 = core.tile(p1);
  Tile& t2 = core.tile(p2);
  if (core.unitAt(p2) || t2.obstacle) {
    return;
  }
  int newcost = t1.cost + getTileCost(u, p1, p2);
  // int ap = getUnitType(u.typeID).actionPoints;
  int ap = u.actionPoints;
  if (t2.cost > newcost && newcost <= ap) {
    q.push(p2, Dir(p2, p1), newcost, Dir(p1, p2));
  }
}

#define FOR_EACH_TILE(p) \
  for (p = V2i(0, 0); core.inboard(p); p = core.incV2i(p))

void Pathfinder::cleanMap() {
  V2i p;
  FOR_EACH_TILE(p) {
    Tile& t = core.tile(p);
    t.cost = 30000;
    t.parent = DirID::NONE;
  }
}

void Pathfinder::tryToPushNeibors(const Unit& u, const V2i& m) {
  assert(core.inboard(m));
  for (int i = 0; i < 6; i++) {
    V2i neibM = Dir::neib(m, static_cast<DirID>(i));
    if (core.inboard(neibM)) {
      processNeibor(u, m, neibM);
    }
  }
}

void Pathfinder::fillMap(const Unit& u) {
  assert(q.isEmpty());
  cleanMap();
  // Push start position
  q.push(u.pos, DirID::NONE, 0, u.dir);
  while (!q.isEmpty()) {
    V2i p = q.pop();
    tryToPushNeibors(u, p);
  }
}

std::vector<V2i> Pathfinder::getPath(const V2i& pos) {
  V2i p = pos;
  std::vector<V2i> path;
  assert(core.inboard(p));
  while (core.tile(p).cost != 0) {
    path.push_back(p);
    p = Dir::neib(p, core.tile(p).parent);
    assert(core.inboard(p));
  }
  // Add start position
  path.push_back(p);
  std::reverse(path.begin(), path.end());
  return path;
}
