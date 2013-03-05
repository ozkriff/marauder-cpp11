// See LICENSE file for copyright and license details.

#include "visualizer/color3u.hpp"

Color3u::Color3u(GLubyte red, GLubyte green, GLubyte blue)
  : mRed(red),
    mGreen(green),
    mBlue(blue)
{}

Color3u::Color3u(GLubyte n)
  : mRed(n),
    mGreen(n),
    mBlue(n)
{}

Color3u::Color3u(int red, int green, int blue)
  : mRed(static_cast<GLubyte>(red)),
    mGreen(static_cast<GLubyte>(green)),
    mBlue(static_cast<GLubyte>(blue))
{}

GLubyte Color3u::red() const {
  return mRed;
}

GLubyte Color3u::green() const {
  return mGreen;
}

GLubyte Color3u::blue() const {
  return mBlue;
}
