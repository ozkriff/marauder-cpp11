/* See LICENSE file for copyright and license details. */

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

void fixnum(int min, int max, int *n);
int rnd(int min, int max);
float deg2rad(float deg);
float rad2deg(float rad);
void fix_angle(float *angle);
bool min_f(float a, float b);
bool max_f(float a, float b);
float dist(const V2f *a, const V2f *b);
void rotate_point_around_point(
    V2f *p, float angle, const V2f *o);
