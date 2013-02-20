// See LICENSE file for copyright and license details.

#include <cassert>
#include <stdexcept>
#include "SDL.h"
#include "SDL_opengl.h"
#include "SDL_image.h"
#include "core/misc.hpp"
#include "ui/gl.hpp"

static GLenum getTextureFormat(
    const SDL_Surface* surface, int nOfColors)
{
  if (nOfColors == 4) {
    // contains an alpha channel
    if (surface->format->Rmask == 0xff) {
      return GL_RGBA;
    } else {
      return GL_BGRA;
    }
  } else if (nOfColors == 3) {
    // no alpha channel
    if (surface->format->Rmask == 0xff) {
      return GL_RGB;
    } else {
      return GL_BGR;
    }
  } else {
    throw std::logic_error("the image is not truecolor..");
  }
}

static void setTextureParameters() {
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,
      GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
}

static bool isPowerOfTwo(int n) {
  return ((n & (n - 1)) == 0);
}

int loadTexture(const std::string& filename) {
  SDL_Surface* surface = IMG_Load(filename.c_str());
  if (!surface) {
    throw std::logic_error(std::string("Can't load image: ") + filename);
  }
  if (!isPowerOfTwo(surface->w) || !isPowerOfTwo(surface->h)) {
    throw std::logic_error("image's height or width is not a power of 2");
  }
  GLint nOfColors = surface->format->BytesPerPixel;
  GLenum textureFormat = getTextureFormat(surface, nOfColors);
  GLuint id;
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  setTextureParameters();
  GLenum target = GL_TEXTURE_2D;
  GLint level = 0;
  GLint internalFormat = nOfColors;
  GLint border = 0;
  GLenum type = GL_UNSIGNED_BYTE;
  glTexImage2D(target, level, internalFormat,
      surface->w, surface->h, border,
      textureFormat, type, surface->pixels);
  SDL_FreeSurface(surface);
  return static_cast<int>(id);
}
