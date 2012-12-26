/* See LICENSE file for copyright and license details. */

#include <cassert>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#include "core/misc.h"
#include "ui_opengl/gl.h"

static GLenum get_texture_format(
    const SDL_Surface *surface, int n_of_colors)
{
  if (n_of_colors == 4) {
    /* contains an alpha channel */
    if (surface->format->Rmask == 0xff) {
      return GL_RGBA;
    } else {
      return GL_BGRA;
    }
  } else if (n_of_colors == 3) {
    /* no alpha channel */
    if (surface->format->Rmask == 0xff) {
      return GL_RGB;
    } else {
      return GL_BGR;
    }
  } else {
    die("gl.c: load_texture(): "
        "the image is not truecolor.."
        "  this will probably break\n");
    return 0;
  }
}

static void set_texture_parameters() {
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

static bool is_power_of_two(int n) {
  return ((n & (n - 1)) == 0);
}

bool load_texture(GLuint *id, const char *filename) {
  GLenum texture_format;
  GLint n_of_colors;
  SDL_Surface *surface;
  assert(id);
  surface = IMG_Load(filename);
  if (!surface) {
    die("gl.c: load_texture(): Can't load file '%s'\n",
        filename);
    return false;
  }
  if (!is_power_of_two(surface->w)
      || !is_power_of_two(surface->h))
  {
    die("ui_opengl/gl.c: load_texture(): "
        "image's height or width is not a power of 2\n");
  }
  n_of_colors = surface->format->BytesPerPixel;
  texture_format = get_texture_format(surface, n_of_colors);
  glGenTextures(1, id);
  glBindTexture(GL_TEXTURE_2D, *id);
  set_texture_parameters();
  glTexImage2D(GL_TEXTURE_2D, 0, n_of_colors,
      surface->w, surface->h, 0,
      texture_format, GL_UNSIGNED_BYTE, surface->pixels);
  if (surface) {
    SDL_FreeSurface(surface);
  }
  return true;
}
