/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include "v2f.h"
#include "line2f.h"

void set_line2f_2v2f(Line2f *line,
    const V2f *a, const V2f *b)
{
  assert(line);
  assert(a);
  assert(b);
  line->a = *a;
  line->b = *b;
}

void set_line2f_4f(Line2f *line,
    float ax, float ay, float bx, float by)
{
  assert(line);
  line->a.x = ax;
  line->a.y = ay;
  line->b.x = bx;
  line->b.y = by;
}

void rotate_line(Line2f *line, float angle) {
  assert(line);
  assert(angle >= 0);
  assert(angle <= 360);
  rotate_v2f(&line->a, angle);
  rotate_v2f(&line->b, angle);
}
