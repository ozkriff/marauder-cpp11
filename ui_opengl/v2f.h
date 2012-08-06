/* See LICENSE file for copyright and license details. */

typedef struct {
  float x, y;
} V2f;

void set_v2f(V2f *v, float x, float y);
void rotate_v2f(V2f *p, float angle);
void v2f_plus(V2f *a, const V2f *b);
