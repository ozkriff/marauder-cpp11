/* See LICENSE file for copyright and license details. */

#ifndef CORE_V2I_H
#define CORE_V2I_H

typedef struct {
  int x, y;
} V2i;

void set_v2i(V2i *v, int x, int y);
bool v2i_is_equal(const V2i *a, const V2i *b);

#endif
