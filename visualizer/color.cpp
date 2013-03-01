// See LICENSE file for copyright and license details.

#include "visualizer/color.hpp"

Color::Color(float red, float green, float blue, float alpha)
  : mRed(red),
    mGreen(green),
    mBlue(blue),
    mAlpha(alpha)
{
}

float Color::red() const {
  return mRed;
}

float Color::green() const {
  return mGreen;
}

float Color::blue() const {
  return mBlue;
}

float Color::alpha() const {
  return mAlpha;
}
