/* See LICENSE file for copyright and license details. */

#include <cassert>
#include <cstdlib>
#include <cmath>
#include "core/math.h"

int dist(const V2i &a, const V2i &b) {
  int dx = abs(b.x - a.x);
  int dy = abs(b.y - a.y);
  return sqrt(pow(dx, 2) + pow(dy, 2));
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
