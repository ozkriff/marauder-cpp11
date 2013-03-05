// See LICENSE file for copyright and license details.

#ifndef VISUALIZER__COLOR3U_HPP
#define VISUALIZER__COLOR3U_HPP

#include "SDL_opengl.h"

class Color3u {
public:
  Color3u(GLubyte red = 255, GLubyte green = 255, GLubyte blue = 255);
  Color3u(int red = 255, int green = 255, int blue = 255);

  GLubyte red() const;
  GLubyte green() const;
  GLubyte blue() const;

private:
  GLubyte mRed, mGreen, mBlue;
};

#endif
