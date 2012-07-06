/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include "bool.h"
#include "v2i.h"

void set_v2i(V2i *v, int x, int y) {
  assert(v);
  v->x = x;
  v->y = y;
}

bool v2i_is_equal(const V2i *a, const V2i *b) {
  assert(a);
  assert(b);
  return a->x == b->x && a->y == b->y;
}
