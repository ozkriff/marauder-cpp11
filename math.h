/* See LICENSE file for copyright and license details. */

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

float get_rot_angle(const V2f *a, const V2f *b);
void clamp_int(int *n, int min, int max);
void clamp_f(float *n, float min, float max);
int rnd(int min, int max);
float deg2rad(float deg);
float rad2deg(float rad);
void clamp_angle(float *angle);
bool min_f(float a, float b);
bool max_f(float a, float b);
float dist(const V2f *a, const V2f *b);
int dist_i(const V2i *a, const V2i *b);
void rotate_point_around_point(
    V2f *p, float angle, const V2f *o);
void swap_int(int *a, int *b);
