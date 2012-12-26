/* See LICENSE file for copyright and license details. */

#include <cstdlib>
#include <cassert>
#include <vector>
#include "core/core.h"
#include "core/path.h"

class PathQueue {
private:
  unsigned int mTailNodeIndex;
  unsigned int mHeadNodeIndex;
  std::vector<V2i> mNodes;

public:
  PathQueue(int size)
    : mTailNodeIndex(0),
      mHeadNodeIndex(0),
      mNodes(size)
  {
    assert(size > 0);
  }

  ~PathQueue() {
  }

  bool is_empty() const {
    return (mHeadNodeIndex == mTailNodeIndex);
  }

  void push(const V2i& m, Dir parent, int newcost, Dir dir) {
    Tile& t = tile(m);
    t.cost = newcost;
    t.parent = parent;
    t.dir = dir;
    mNodes[mTailNodeIndex] = m;
    mTailNodeIndex++;
    if (mTailNodeIndex == mNodes.size()) {
      mTailNodeIndex = 0;
    }
  }

  V2i pop() {
    assert(mHeadNodeIndex != mTailNodeIndex);
    V2i m = mNodes[mHeadNodeIndex];
    mHeadNodeIndex++;
    if (mHeadNodeIndex == mNodes.size()) {
      mHeadNodeIndex = 0;
    }
    return m;
  }
};

static PathQueue q(1000);

void init_pathfinding_module() {
}

/* If this is first(start) tile - no parent tile. */
static Dir get_parent_dir (const Unit& u, const V2i& m) {
  Tile& t = tile(m);
  if (t.cost == 0) {
    return u.dir;
  } else {
    return opposite_dir(t.parent);
  }
}

static int get_tile_cost(const Unit& u, const V2i& t, const V2i& nb) {
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
  Dir d = m2dir(t, nb);
  Dir d2 = get_parent_dir(u, t);
  int d_diff = dir_diff(d, d2);
  int additionalCost[] = {0, 3, 20, 90, 90};
  assert(d_diff >= 0 && d_diff <= 4);
  return cost + additionalCost[d_diff];
}

static bool can_move_there(const V2i& p1, const V2i& p2) {
  assert(inboard(p1));
  assert(inboard(p2));
  if (!dir_is_diagonal(m2dir(p1, p2))) {
    return true;
  }
  V2i neib_left = get_dir_neib(p1, p2, -1);
  V2i neib_right = get_dir_neib(p1, p2, +1);
  bool is_left_blocked = inboard(neib_left) && tile(neib_left).obstacle;
  bool is_right_blocked = inboard(neib_right) && tile(neib_right).obstacle;
#if 0
  return !is_left_blocked && !is_right_blocked;
#else
  return !is_left_blocked || !is_right_blocked;
#endif
}

/* TODO rename */
/* p1 - orig_pos, p2 - neib pos */
static void process_neibor(const Unit& u, const V2i& p1, const V2i& p2) {
  Tile& t1 = tile(p1);
  Tile& t2 = tile(p2);
  if (unit_at(p2) || t2.obstacle || !can_move_there(p1, p2)) {
    return;
  }
  int newcost = t1.cost + get_tile_cost(u, p1, p2);
  int ap = get_unit_type(u.type_id).action_points;
  if (t2.cost > newcost && newcost <= ap) {
    q.push(p2, m2dir(p2, p1), newcost, m2dir(p1, p2));
  }
}

void clean_map() {
  V2i p;
  FOR_EACH_TILE(&p) {
    Tile& t = tile(p);
    t.cost = 30000;
    t.parent = Dir::D_NONE;
  }
}

static void try_to_push_neibors(const Unit& u, const V2i& m) {
  assert(inboard(m));
  for (int i = static_cast<int>(Dir::D_N);
      i <= static_cast<int>(Dir::D_NW);
      i++)
  {
    V2i neib_m = neib(m, static_cast<Dir>(i));
    if (inboard(neib_m)) {
      process_neibor(u, m, neib_m);
    }
  }
}

void fill_map(const Unit& u) {
  assert(q.is_empty());
  clean_map();
  // Push start position
  q.push(u.pos, Dir::D_NONE, 0, u.dir);
  while (!q.is_empty()) {
    V2i p = q.pop();
    try_to_push_neibors(u, p);
  }
}

void get_path(V2i *path, int length, V2i pos) {
  Dir dir;
  int i = length - 1;
  assert(path);
  assert(inboard(pos));
  while (tile(pos).cost != 0) {
    path[i] = pos;
    dir = tile(pos).parent;
    pos = neib(pos, dir);
    i--;
  }
  /* Add start position. */
  path[i] = pos;
}

int get_path_length(const V2i& pos) {
  V2i p = pos;
  int length = 1;
  assert(inboard(p));
  while (tile(p).cost != 0) {
    length++;
    Dir dir = tile(p).parent;
    p = neib(p, dir);
  }
  return length;
}
