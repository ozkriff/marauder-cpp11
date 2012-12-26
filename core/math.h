/* See LICENSE file for copyright and license details. */

#ifndef CORE_MATH_H
#define CORE_MATH_H

#include "core/v2i.h"

int dist(const V2i& a, const V2i& b);
int rnd(int min, int max);
void clamp_int(int *n, int min, int max);
void swap_int(int *a, int *b);

#endif
