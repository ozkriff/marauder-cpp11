/* See LICENSE file for copyright and license details. */

#include <math.h>
#include <assert.h>
#include "ui_opengl/v2f.h"

void set_v2f(V2f *v, float x, float y) {
  assert(v);
  v->x = x;
  v->y = y;
}

void rotate_v2f(V2f *p, float angle) {
  V2f old;
  float sn, cs;
  assert(p);
  assert(angle >= 0);
  assert(angle <= 360);
  old = *p;
  sn = sin(angle);
  cs = cos(angle);
  p->x = cs * old.x - sn * old.y;
  p->y = sn * old.x + cs * old.y;
}

void v2f_plus(V2f *a, const V2f *b) {
  assert(a);
  assert(b);
  a->x += b->x;
  a->y += b->y;
}
