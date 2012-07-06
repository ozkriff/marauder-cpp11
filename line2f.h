/* See LICENSE file for copyright and license details. */

typedef struct {
  V2f a, b;
} Line2f;

void set_line2f_2v2f(Line2f *line,
    const V2f *a, const V2f *b);
void set_line2f_4f(Line2f *line,
    float ax, float ay, float bx, float by);
void rotate_line(Line2f *line, float angle);
