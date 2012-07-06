/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include "bool.h"
#include "list.h"
#include "misc.h"
#include "v2i.h"
#include "dir.h"

/* TODO rename! */
int directions[D_COUNT - 1][2] = {
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
  for (i = 0; i < 16; i++) {
    if (dx == directions[i][0] && dy == directions[i][1])
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
    die("misc: neib(): "
        "Wrong direction: pos:[%d %d %d] dir:%d\n",
        pos->x, pos->y, i);
  }
  dx = directions[i][0];
  dy = directions[i][1];
  set_v2i(neib, pos->x + dx, pos->y + dy);
}
