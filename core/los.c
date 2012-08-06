/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "core/bool.h"
#include "core/v2i.h"
#include "core/los.h"
#include "core/math.h"

/* Wikipedia: Bresenham's line algorithm */

void los_init(LosData *data, const V2i *a, const V2i *b) {
  assert(data);
  assert(a);
  assert(b);
  data->a = *a;
  data->b = *b;
  data->is_steep = abs(b->y - a->y) > abs(b->x - a->x);
  if (data->is_steep) {
    swap_int(&data->a.x, &data->a.y);
    swap_int(&data->b.x, &data->b.y);
  }
  if (data->a.x > data->b.x) {
    swap_int(&data->a.x, &data->b.x);
    swap_int(&data->a.y, &data->b.y);
  }
  data->delta.x = data->b.x - data->a.x;
  data->delta.y = abs(data->b.y - data->a.y);
  data->error = data->delta.x >> 1;
  data->pos = data->a;
  data->ystep = (data->a.y < data->b.y) ? 1 : -1;
}

bool los_is_finished(LosData *data) {
  assert(data);
  return data->pos.x >= data->b.x;
}

void los_get_next(LosData *data, V2i *next_pos) {
  assert(data);
  assert(next_pos);
  data->error -= data->delta.y;
  if (data->error < 0) {
    data->pos.y += data->ystep;
    data->error += data->delta.x;
  }
  data->pos.x++;
  if (!data->is_steep) {
    set_v2i(next_pos, data->pos.x, data->pos.y);
  } else {
    set_v2i(next_pos, data->pos.y, data->pos.x);
  }
}
