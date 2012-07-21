/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#include "bool.h"
#include "gl.h"
#include "list.h"
#include "misc.h"

bool load_texture(GLuint *id, const char *filename) {
  GLenum texture_format;
  GLint n_of_colors;
  SDL_Surface *surface;
  assert(id);
  surface = IMG_Load(filename);
  if (surface) {
    if ((surface->w & (surface->w - 1)) != 0) {
      die("gl.c: load_texture(): "
          "image's width is not a power of 2\n");
    }
    if ((surface->h & (surface->h - 1)) != 0) {
      die("gl.c: load_texture(): "
          "image's height is not a power of 2\n");
    }
    n_of_colors = surface->format->BytesPerPixel;
    if (n_of_colors == 4) {
      /* contains an alpha channel */
      if (surface->format->Rmask == 0x000000ff) {
        texture_format = GL_RGBA;
      } else {
        texture_format = GL_BGRA;
      }
    } else if (n_of_colors == 3) {
      /* no alpha channel */
      if (surface->format->Rmask == 0x000000ff) {
        texture_format = GL_RGB;
      } else {
        texture_format = GL_BGR;
      }
    } else {
      die("gl.c: load_texture(): "
          "the image is not truecolor.."
          "  this will probably break\n");
    }
    glGenTextures(1, id);
    glBindTexture(GL_TEXTURE_2D, *id);
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
    glTexImage2D(GL_TEXTURE_2D, 0, n_of_colors,
        surface->w, surface->h, 0,
        texture_format, GL_UNSIGNED_BYTE, surface->pixels);
  } else {
    die("gl.c: load_texture(): Can't load file '%s'\n",
        filename);
    return false;
  }
  if (surface) {
    SDL_FreeSurface(surface);
  }
  return true;
}
