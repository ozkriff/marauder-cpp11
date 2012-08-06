/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include <stdlib.h>
#include "core/bool.h"
#include "core/list.h"
#include "core/misc.h"
#include "core/v2i.h"
#include "core/dir.h"

V2i dir_to_pos_diff[8] = {
  {1, 0},
  {1, 1},
  {0, 1},
  {-1, 1},
  {-1, 0},
  {-1, -1},
  {0, -1},
  {1, -1},
};

Dir m2dir(const V2i *a, const V2i *b) {
  int dx, dy;
  int i;
  assert(a);
  assert(b);
  dx = b->x - a->x;
  dy = b->y - a->y;
  for (i = 0; i < 8; i++) {
    if (dx == dir_to_pos_diff[i].x && dy == dir_to_pos_diff[i].y)
      return (Dir)i;
  }
  return D_ERROR;
}

/* Get tile's neiborhood by it's index. */
void neib(V2i *neib, const V2i* pos, Dir i) {
  int dx, dy;
  assert(i < D_COUNT);
  assert(pos);
  assert(neib);
  if (i == D_NONE || i == D_ERROR) {
    die("dir.c: neib(): "
        "Wrong direction: pos:[%d %d %d] dir:%d\n",
        pos->x, pos->y, i);
  }
  dx = dir_to_pos_diff[i].x;
  dy = dir_to_pos_diff[i].y;
  set_v2i(neib, pos->x + dx, pos->y + dy);
}

bool dir_is_diagonal(Dir d) {
  return d != D_N && d != D_E && d != D_S && d != D_W;
}

/* TODO rename */
void get_dir_neib(
    V2i *n, const V2i *p1, const V2i *p2, int add_me)
{
  int d;
  assert(n);
  assert(p1);
  assert(p2);
  assert(add_me >= -7);
  assert(add_me <= 7);
  d = (int)m2dir(p1, p2) + add_me;
  while (d > 7) {
    d -= 8;
  }
  while (d < 0) {
    d += 8;
  }
  assert(d >= 0 && d <= 7);
  neib(n, p1, (Dir)d);
}

Dir opposite_dir(Dir d) {
  int tmp = (int)d + 4;
  if (tmp >= 8) {
    tmp -= 8;
  }
  return (Dir)tmp;
}

int dir_diff(Dir d0, Dir d1) {
  int diff = abs(d0 - d1);
  if (diff > 4) {
    diff = 8 - diff;
  }
  return diff;
}
