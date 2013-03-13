// See LICENSE file for copyright and license details.

#include "visualizer/color3u.hpp"

Color3u::Color3u(UnsignedByte red, UnsignedByte green, UnsignedByte blue)
  : mRed(red),
    mGreen(green),
    mBlue(blue)
{}

Color3u::Color3u(UnsignedByte n)
  : mRed(n),
    mGreen(n),
    mBlue(n)
{}

Color3u::Color3u(int red, int green, int blue)
  : mRed(static_cast<UnsignedByte>(red)),
    mGreen(static_cast<UnsignedByte>(green)),
    mBlue(static_cast<UnsignedByte>(blue))
{}

UnsignedByte Color3u::red() const {
  return mRed;
}

UnsignedByte Color3u::green() const {
  return mGreen;
}

UnsignedByte Color3u::blue() const {
  return mBlue;
}
