// See LICENSE file for copyright and license details.

#include "ui/color.hpp"

Color::Color(float red, float green, float blue)
  : mRed(red),
    mGreen(green),
    mBlue(blue)
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

