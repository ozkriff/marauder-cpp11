// See LICENSE file for copyright and license details.

#ifndef VISUALIZER__MATH_HPP
#define VISUALIZER__MATH_HPP

#include "visualizer/v2f.hpp"
#include "visualizer/v3f.hpp"

class Direction;

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

float getRotAngle(const V2f& a, const V2f& b);
float clampF(float n, float min, float max);
float deg2rad(float deg);
float rad2deg(float rad);
float clampAngle(float angle);
bool minF(float a, float b);
bool maxF(float a, float b);
float dist(const V2f& a, const V2f& b);
void rotatePointAroundPoint(
    V2f *p, float angle, const V2f *o);
float dirToAngle(const Direction& direction);

#endif
