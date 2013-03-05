// See LICENSE file for copyright and license details.

#ifndef VISUALIZER__COLOR3U_HPP
#define VISUALIZER__COLOR3U_HPP

#include "SDL_opengl.h"

class Color3u {
public:
  Color3u(GLubyte red, GLubyte green, GLubyte blue);
  Color3u(GLubyte n);
  Color3u(int red, int green, int blue);

  GLubyte red() const;
  GLubyte green() const;
  GLubyte blue() const;

private:
  GLubyte mRed, mGreen, mBlue;
};

#endif
