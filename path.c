/* See LICENSE file for copyright and license details. */

#include <stdlib.h>
#include <assert.h>
#include "bool.h"
#include "list.h"
#include "v2i.h"
#include "misc.h"
#include "dir.h"
#include "game.h"

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

static void push(const V2i *m, Dir parent, float newcost) {
  Tile *t;
  assert(m);
  t = tile(m);
  assert(t);
  t->cost = newcost;
  t->parent = parent;
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

static int get_tile_cost(const V2i *t, const V2i *nb) {
  float cost = 1;
  int dx, dy;
  assert(t);
  assert(nb);
  dx = abs(t->x - nb->x);
  dy = abs(t->y - nb->y);
  assert(dx <= 1);
  assert(dy <= 1);
  if (dx != 0)
    cost++;
  if (dy != 0)
    cost++;
  return cost;
}

static bool can_move_there(const V2i *p1, const V2i *p2) {
  V2i neib_left, neib_right;
  Dir d;
  assert(p1);
  assert(p2);
  d = m2dir(p1, p2);
  if (!dir_is_diagonal(d)) {
    return true;
  }
  get_dir_neib(&neib_left, p1, p2, -1);
  get_dir_neib(&neib_right, p1, p2, +1);
#if 1
  if ((inboard(&neib_left) && tile(&neib_left)->obstacle)
      || (inboard(&neib_right) && tile(&neib_right)->obstacle))
#else
  if ((inboard(&neib_left) && tile(&neib_left)->obstacle)
      && (inboard(&neib_right) && tile(&neib_right)->obstacle))
#endif
  {
    return false;
  }
  return true;
}

/* TODO rename */
/* p1 - orig_pos, p2 - neib pos */
static void process_neibor(const V2i *p1, const V2i *p2) {
  int newcost;
  Tile *t1 = tile(p1);
  Tile *t2 = tile(p2);
  assert(t1);
  assert(t2);
  if (unit_at(p2) || t2->obstacle
      || !can_move_there(p1, p2))
  {
    return;
  }
  newcost = t1->cost + get_tile_cost(p1, p2);
  if (t2->cost > newcost && newcost <= action_points) {
    push(p2, m2dir(p2, p1), newcost);
  }
}

void clean_map(void) {
  V2i p;
  for (set_v2i(&p, 0, 0); inboard(&p); inc_v2i(&p)) {
    Tile *t = tile(&p);
    assert(t);
    t->cost = 30000;
    t->parent = D_NONE;
  }
}

static void try_to_push_neibors(const V2i *m) {
  Dir i;
  assert(m);
  assert(inboard(m));
  for (i = D_N; i <= D_NW; i++) {
    V2i neib_m;
    neib(&neib_m, m, i);
    if (inboard(&neib_m)) {
      process_neibor(m, &neib_m);
    }
  }
}

void fill_map(const Unit *u) {
  assert(u);
  assert(is_queue_empty(&q));
  clean_map();
  push(&u->pos, D_NONE, 0); /* Push start position. */
  while (!is_queue_empty(&q)) {
    V2i p = pop();
    try_to_push_neibors(&p);
  }
}

/* TODO rewrite */
List get_path(V2i pos) {
  List path = {NULL, NULL, 0};
  Dir dir;
  assert(inboard(&pos));
  while (tile(&pos)->cost != 0) {
    push_node(&path, COPY_TO_HEAP(&pos, V2i));
    dir = tile(&pos)->parent;
    neib(&pos, &pos, dir);
  }
  /* Add start position. */
  push_node(&path, COPY_TO_HEAP(&pos, V2i));
  return path;
}
