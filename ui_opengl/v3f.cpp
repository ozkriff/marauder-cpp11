// See LICENSE file for copyright and license details.

#include <cassert>
#include "ui_opengl/v3f.h"

void setV3f(V3f *v, float x, float y, float z) {
  assert(v);
  v->x = x;
  v->y = y;
  v->z = z;
}
