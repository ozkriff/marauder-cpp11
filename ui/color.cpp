// See LICENSE file for copyright and license details.

#include "ui/color.hpp"

Color::Color(float red, float green, float blue)
  : mRed(red),
    mGreen(green),
    mBlue(blue)
{
}

float Color::red() {
  return mRed;
}

float Color::green() {
  return mGreen;
}

float Color::blue() {
  return mBlue;
}

