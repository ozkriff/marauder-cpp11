/* See LICENSE file for copyright and license details. */

#include <cassert>
#include <cstdlib>
#include "core/dir.h"

static V2i dir_to_pos_diff[8] = {
  {1, 0},
  {1, 1},
  {0, 1},
  {-1, 1},
  {-1, 0},
  {-1, -1},
  {0, -1},
  {1, -1},
};

Dir m2dir(const V2i& a, const V2i& b) {
  int dx = b.x - a.x;
  int dy = b.y - a.y;
  for (int i = 0; i < 8; i++) {
    if (dx == dir_to_pos_diff[i].x
        && dy == dir_to_pos_diff[i].y)
    {
      return static_cast<Dir>(i);
    }
  }
  return Dir::D_ERROR;
}

/* Get tile's neiborhood by it's index. */
V2i neib(const V2i& pos, Dir i) {
  assert(i < Dir::D_COUNT);
  if (i == Dir::D_NONE || i == Dir::D_ERROR) {
    die("dir.c: neib(): "
        "Wrong direction: pos:[%d %d %d] dir:%d\n",
        pos.x, pos.y, i);
  }
  int dx = dir_to_pos_diff[static_cast<int>(i)].x;
  int dy = dir_to_pos_diff[static_cast<int>(i)].y;
  return V2i(pos.x + dx, pos.y + dy);
}

bool dir_is_diagonal(Dir d) {
  return d != Dir::D_N && d != Dir::D_E && d != Dir::D_S && d != Dir::D_W;
}

/* TODO rename */
V2i get_dir_neib(const V2i& p1, const V2i& p2, int add_me) {
  assert(add_me >= -7);
  assert(add_me <= 7);
  int d = static_cast<int>(m2dir(p1, p2)) + add_me;
  while (d > 7) {
    d -= 8;
  }
  while (d < 0) {
    d += 8;
  }
  assert(d >= 0 && d <= 7);
  return neib(p1, static_cast<Dir>(d));
}

Dir opposite_dir(Dir d) {
  int tmp = (int)d + 4;
  if (tmp >= 8) {
    tmp -= 8;
  }
  return (Dir)tmp;
}

int dir_diff(Dir d0, Dir d1) {
  int diff = abs((int)d0 - (int)d1);
  if (diff > 4) {
    diff = 8 - diff;
  }
  return diff;
}
