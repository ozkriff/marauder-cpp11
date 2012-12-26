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

bool PathQueue::is_empty() const {
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
Dir Pathfinder::get_parent_dir(const Unit& u, const V2i& m) {
  Tile& tile = core.tile(m);
  if (tile.cost == 0) {
    return u.dir;
  } else {
    return tile.parent.opposite();
  }
}

int Pathfinder::get_tile_cost(const Unit& u, const V2i& t, const V2i& nb) {
  int cost = 1;
  int dx = abs(t.x - nb.x);
  int dy = abs(t.y - nb.y);
  assert(dx <= 1);
  assert(dy <= 1);
  if (dx != 0) {
    cost++;
  }
  if (dy != 0) {
    cost++;
  }
  Dir d(t, nb);
  Dir d2 = get_parent_dir(u, t);
  int d_diff = d.diff(d2);
  int additionalCost[] = {0, 3, 20, 90, 90};
  assert(d_diff >= 0 && d_diff <= 4);
  return cost + additionalCost[d_diff];
}

bool Pathfinder::can_move_there(const V2i& p1, const V2i& p2) {
  assert(core.inboard(p1));
  assert(core.inboard(p2));
  assert(dist(p1, p2) == 1);
  if (!Dir(p1, p2).isDiagonal()) {
    return true;
  }
  V2i neib_left = Dir::get_neib(p1, p2, -1);
  V2i neib_right = Dir::get_neib(p1, p2, +1);
  bool is_left_blocked = core.inboard(neib_left)
      && core.tile(neib_left).obstacle;
  bool is_right_blocked = core.inboard(neib_right)
      && core.tile(neib_right).obstacle;
#if 0
  return !is_left_blocked && !is_right_blocked;
#else
  return !is_left_blocked || !is_right_blocked;
#endif
}

// TODO: rename
// p1 - orig_pos, p2 - neib pos
void Pathfinder::process_neibor(const Unit& u, const V2i& p1, const V2i& p2) {
  Tile& t1 = core.tile(p1);
  Tile& t2 = core.tile(p2);
  if (core.unit_at(p2) || t2.obstacle || !can_move_there(p1, p2)) {
    return;
  }
  int newcost = t1.cost + get_tile_cost(u, p1, p2);
  int ap = get_unit_type(u.type_id).action_points;
  if (t2.cost > newcost && newcost <= ap) {
    q.push(p2, Dir(p2, p1), newcost, Dir(p1, p2));
  }
}

#define FOR_EACH_TILE(p) \
  for (*p = V2i(0, 0); core.inboard(*p); core.inc_v2i(p))

void Pathfinder::clean_map() {
  V2i p;
  FOR_EACH_TILE(&p) {
    Tile& t = core.tile(p);
    t.cost = 30000;
    t.parent = DirID::D_NONE;
  }
}

void Pathfinder::try_to_push_neibors(const Unit& u, const V2i& m) {
  assert(core.inboard(m));
  for (int i = Dir(DirID::D_N).toInt(); i <= Dir(DirID::D_NW).toInt(); i++) {
    V2i neib_m = Dir::neib(m, static_cast<DirID>(i));
    if (core.inboard(neib_m)) {
      process_neibor(u, m, neib_m);
    }
  }
}

void Pathfinder::fill_map(const Unit& u) {
  assert(q.is_empty());
  clean_map();
  // Push start position
  q.push(u.pos, DirID::D_NONE, 0, u.dir);
  while (!q.is_empty()) {
    V2i p = q.pop();
    try_to_push_neibors(u, p);
  }
}

std::vector<V2i> Pathfinder::get_path(const V2i& pos) {
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
