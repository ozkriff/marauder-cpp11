// See LICENSE file for copyright and license details.

#include <cassert>
#include <cstdlib>
#include <cmath>
#include "core/math.hpp"

int rnd(int min, int max) {
  assert(min < max);
  max++;
  if (max != min) {
    return rand() % (max - min) + min;
  } else {
    return max;
  }
}

void clampInt(int *n, int min, int max) {
  assert(n);
  if (*n < min) {
    *n = min;
  } else if (*n > max) {
    *n = max;
  }
}

bool isPowerOfTwo(int n) {
  return ((n & (n - 1)) == 0);
}
