/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "bool.h"
#include "v2i.h"
#include "v2f.h"
#include "los.h"
#include "math.h"

/* Wikipedia: Bresenham's line algorithm */

void los_init(LosData *br, const V2i *a, const V2i *b) {
  assert(br);
  assert(a);
  assert(b);
  br->a = *a;
  br->b = *b;
  br->is_steep = abs(b->y - a->y) > abs(b->x - a->x);
  if (br->is_steep) {
    swap_int(&br->a.x, &br->a.y);
    swap_int(&br->b.x, &br->b.y);
  }
  if (br->a.x > br->b.x) {
    swap_int(&br->a.x, &br->b.x);
    swap_int(&br->a.y, &br->b.y);
  }
  br->delta.x = br->b.x - br->a.x;
  br->delta.y = abs(br->b.y - br->a.y);
  br->error = br->delta.x >> 1;
  br->pos = br->a;
  br->ystep = (br->a.y < br->b.y) ? 1 : -1;
}

bool los_is_finished(LosData *br) {
  assert(br);
  return br->pos.x >= br->b.x;
}

void los_get_next(LosData *br, V2i *next_pos) {
  assert(br);
  assert(next_pos);
  br->error -= br->delta.y;
  if (br->error < 0) {
    br->pos.y += br->ystep;
    br->error += br->delta.x;
  }
  br->pos.x++;
  if (!br->is_steep) {
    set_v2i(next_pos, br->pos.x, br->pos.y);
  } else {
    set_v2i(next_pos, br->pos.y, br->pos.x);
  }
}
