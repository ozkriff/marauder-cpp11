// See LICENSE file for copyright and license details.

#include <cassert>
#include <stdexcept>
#include "SDL.h"
#include "SDL_opengl.h"
#include "SDL_image.h"
#include "core/misc.hpp"
#include "core/math.hpp"
#include "visualizer/texture.hpp"

namespace {

GLenum getTextureFormat(
    const SDL_Surface* surface, int bytesPerPixel)
{
  if (bytesPerPixel == 4) {
    // contains an alpha channel
    if (surface->format->Rmask == 0xff) {
      return GL_RGBA;
    } else {
      return GL_BGRA;
    }
  } else if (bytesPerPixel == 3) {
    // no alpha channel
    if (surface->format->Rmask == 0xff) {
      return GL_RGB;
    } else {
      return GL_BGR;
    }
  } else {
    throw std::runtime_error("the image is not truecolor..");
  }
}

void setTextureParameters() {
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,
      GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
}

} // namespace

int loadTexture(const std::string& filename) {
  SDL_Surface* surface = IMG_Load(filename.c_str());
  if (!surface) {
    throw std::runtime_error(std::string("Can't load image: ") + filename);
  }
  if (!isPowerOfTwo(surface->w) || !isPowerOfTwo(surface->h)) {
    throw std::runtime_error("image's height or width is not a power of 2");
  }
  GLint bytesPerPixel = surface->format->BytesPerPixel;
  GLenum textureFormat = getTextureFormat(surface, bytesPerPixel);
  UnsignedInteger id;
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  setTextureParameters();
  GLenum target = GL_TEXTURE_2D;
  GLint level = 0;
  GLint internalFormat = bytesPerPixel;
  GLint border = 0;
  GLenum type = GL_UNSIGNED_BYTE;
  glTexImage2D(target, level, internalFormat,
      surface->w, surface->h, border,
      textureFormat, type, surface->pixels);
  SDL_FreeSurface(surface);
  return static_cast<int>(id);
}
