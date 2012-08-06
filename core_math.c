/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "bool.h"
#include "v2i.h"
#include "core_math.h"

int dist_i(const V2i *a, const V2i *b) {
  int dx, dy;
  assert(a);
  assert(b);
  dx = abs(b->x - a->x);
  dy = abs(b->y - a->y);
  return (int)sqrt(pow(dx, 2) + pow(dy, 2));
}

int rnd(int min, int max) {
  assert(min < max);
  max++;
  if (max != min) {
    return rand() % (max - min) + min;
  } else {
    return max;
  }
}

void clamp_int(int *n, int min, int max) {
  assert(n);
  if (*n < min) {
    *n = min;
  } else if (*n > max) {
    *n = max;
  }
}

void swap_int(int *a, int *b) {
  int tmp;
  assert(a);
  assert(b);
  tmp = *a;
  *a = *b;
  *b = tmp;
}
