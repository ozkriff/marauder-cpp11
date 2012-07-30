/* See LICENSE file for copyright and license details. */

#include <stdlib.h>
#include <assert.h>
#include "bool.h"
#include "list.h"
#include "v2i.h"
#include "misc.h"
#include "dir.h"
#include "game.h"
#include "unit_type.h"

typedef struct {
  V2i *v;
  int tail;
  int head;
  int size;
} PathQueue;

static PathQueue q;

void init_path_queue (PathQueue *q, int size) {
  assert(size > 0);
  assert(q);
  q->v = ALLOCATE(size, V2i);
  q->tail = 0;
  q->head = 0;
  q->size = size;
}

void init_pathfinding_module(void) {
  init_path_queue(&q, 10000);
}

static void push(
    const V2i *m, Dir parent, float newcost, Dir dir)
{
  Tile *t;
  assert(m);
  t = tile(m);
  assert(t);
  t->cost = newcost;
  t->parent = parent;
  t->dir = dir;
  q.v[q.tail] = *m;
  q.tail++;
  if (q.tail == q.size)
    q.tail = 0;
}

static bool is_queue_empty(const PathQueue *q) {
  assert(q);
  return q->head == q->tail;
}

static V2i pop(void) {
  V2i m;
  assert(q.head != q.tail);
  m = q.v[q.head];
  q.head++;
  if (q.head == q.size)
    q.head = 0;
  return m;
}

/* If this is first(start) tile - no parent tile. */
static Dir get_parent_dir (const Unit *u, const V2i *m){
  Tile *t = tile(m);
  if (t->cost == 0) {
    return u->dir;
  } else {
    return opposite_dir(t->parent);
  }
}

static int get_tile_cost(
    const Unit *u, const V2i *t, const V2i *nb)
{
  float cost = 1;
  int dx, dy;
  Dir d, d2, d_diff;
  assert(t);
  assert(nb);
  assert(u);
  dx = abs(t->x - nb->x);
  dy = abs(t->y - nb->y);
  assert(dx <= 1);
  assert(dy <= 1);
  if (dx != 0)
    cost++;
  if (dy != 0)
    cost++;
  d = m2dir(t, nb);
  d2 = get_parent_dir(u, t);
  d_diff = dir_diff(d, d2);
  switch (d_diff) {
    case 0: break;
    case 1: cost += 3; break;
    case 2: cost += 20; break;
    case 3: cost += 90; break;
    case 4: cost += 90; break;
    default: exit(1); break;
  }
  return cost;
}

static bool can_move_there(const V2i *p1, const V2i *p2) {
  V2i neib_left, neib_right;
  bool is_left_blocked, is_right_blocked;
  assert(p1);
  assert(p2);
  assert(inboard(p1));
  assert(inboard(p2));
  if (!dir_is_diagonal(m2dir(p1, p2))) {
    return true;
  }
  get_dir_neib(&neib_left, p1, p2, -1);
  get_dir_neib(&neib_right, p1, p2, +1);
  is_left_blocked = inboard(&neib_left)
      && tile(&neib_left)->obstacle;
  is_right_blocked = inboard(&neib_right)
      && tile(&neib_right)->obstacle;
#if 0
  return !is_left_blocked && !is_right_blocked;
#else
  return !is_left_blocked || !is_right_blocked;
#endif
}

/* TODO rename */
/* p1 - orig_pos, p2 - neib pos */
static void process_neibor(
    const Unit *u, const V2i *p1, const V2i *p2)
{
  int newcost;
  int ap;
  Tile *t1 = tile(p1);
  Tile *t2 = tile(p2);
  assert(t1);
  assert(t2);
  assert(u);
  if (unit_at(p2) || t2->obstacle
      || !can_move_there(p1, p2))
  {
    return;
  }
  newcost = t1->cost + get_tile_cost(u, p1, p2);
  ap = get_unit_type(u->type_id)->action_points;
  if (t2->cost > newcost && newcost <= ap) {
    push(p2, m2dir(p2, p1), newcost, m2dir(p1, p2));
  }
}

void clean_map(void) {
  V2i p;
  FOR_EACH_TILE(&p) {
    Tile *t = tile(&p);
    assert(t);
    t->cost = 30000;
    t->parent = D_NONE;
  }
}

static void try_to_push_neibors(
    const Unit *u, const V2i *m)
{
  Dir i;
  assert(m);
  assert(inboard(m));
  assert(u);
  for (i = D_N; i <= D_NW; i++) {
    V2i neib_m;
    neib(&neib_m, m, i);
    if (inboard(&neib_m)) {
      process_neibor(u, m, &neib_m);
    }
  }
}

void fill_map(const Unit *u) {
  assert(u);
  assert(is_queue_empty(&q));
  clean_map();
  /* Push start position. */
  push(&u->pos, D_NONE, 0, u->dir);
  while (!is_queue_empty(&q)) {
    V2i p = pop();
    try_to_push_neibors(u, &p);
  }
}

/* TODO rewrite */
void get_path(List *path, V2i pos) {
  Dir dir;
  assert(path);
  assert(!path->head);
  assert(!path->tail);
  assert(path->count == 0);
  assert(inboard(&pos));
  while (tile(&pos)->cost != 0) {
    push_node(path, COPY_TO_HEAP(&pos, V2i));
    dir = tile(&pos)->parent;
    neib(&pos, &pos, dir);
  }
  /* Add start position. */
  push_node(path, COPY_TO_HEAP(&pos, V2i));
}
