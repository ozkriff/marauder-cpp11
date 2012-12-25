/* See LICENSE file for copyright and license details. */

#ifndef CORE_LOS_H
#define CORE_LOS_H

typedef struct {
  V2i a, b;
  V2i delta;
  V2i pos; /* current position */
  int ystep;
  int error;
  bool is_steep;
} LosData;

void los_init(LosData *br, const V2i *a, const V2i *b);
bool los_is_finished(LosData *br);
void los_get_next(LosData *br, V2i *next_pos);

#endif
