// See LICENSE file for copyright and license details.

#include <cassert>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#include "core/misc.hpp"
#include "ui_opengl/gl.hpp"

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
    die("gl.c: loadTexture(): "
        "the image is not truecolor.."
        "  this will probably break\n");
    return 0;
  }
}

static void setTextureParameters() {
  glTexParameteri(GL_TEXTURE_2D,
      GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,
      GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,
      GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,
      GL_TEXTURE_MIN_FILTER,
      GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,
      GL_GENERATE_MIPMAP, GL_TRUE);
}

static bool isPowerOfTwo(int n) {
  return ((n & (n - 1)) == 0);
}

int loadTexture(const char *filename) {
  GLuint id;
  SDL_Surface* surface = IMG_Load(filename);
  if (!surface) {
    die("gl.c: loadTexture(): Can't load file '%s'\n",
        filename);
    return 0;
  }
  if (!isPowerOfTwo(surface->w)
      || !isPowerOfTwo(surface->h))
  {
    die("ui_opengl/gl.c: loadTexture(): "
        "image's height or width is not a power of 2\n");
  }
  GLint nOfColors = surface->format->BytesPerPixel;
  GLenum textureFormat = getTextureFormat(surface, nOfColors);
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  setTextureParameters();
  glTexImage2D(GL_TEXTURE_2D, 0, nOfColors,
      surface->w, surface->h, 0,
      textureFormat, GL_UNSIGNED_BYTE, surface->pixels);
  if (surface) {
    SDL_FreeSurface(surface);
  }
  return static_cast<int>(id);
}
