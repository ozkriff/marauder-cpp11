// See LICENSE file for copyright and license details.

#include "visualizer/color4f.hpp"

Color4f::Color4f(float red, float green, float blue, float alpha)
  : mRed(red),
    mGreen(green),
    mBlue(blue),
    mAlpha(alpha)
{
}

float Color4f::red() const {
  return mRed;
}

float Color4f::green() const {
  return mGreen;
}

float Color4f::blue() const {
  return mBlue;
}

float Color4f::alpha() const {
  return mAlpha;
}
