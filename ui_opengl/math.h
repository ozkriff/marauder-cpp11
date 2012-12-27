// See LICENSE file for copyright and license details.

#ifndef UI_OPENGL_MATH_H
#define UI_OPENGL_MATH_H

#include "ui_opengl/v2f.h"
#include "ui_opengl/v3f.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

float getRotAngle(const V2f& a, const V2f& b);
void clampF(float *n, float min, float max);
float deg2rad(float deg);
float rad2deg(float rad);
void clampAngle(float *angle);
bool minF(float a, float b);
bool maxF(float a, float b);
float dist(const V2f& a, const V2f& b);
void rotatePointAroundPoint(
    V2f *p, float angle, const V2f *o);

#endif
