/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include <SDL/SDL_opengl.h>
#include "va.h"

const Va empty_va = {NULL, NULL, NULL, 0};

void set_xy(float *coords, int n, int i, int vi,
    float x, float y)
{
  float *coord;
  assert(n == 2 || n == 3 || n == 4);
  assert(vi >= 0 && vi <= n);
  assert(i >= 0);
  coord = coords + (i * n * 2) + (2 * vi);
  *(coord + 0) = x;
  *(coord + 1) = y;
}

/*if(quads) n=4; if(tris) n=3*/
/* i - polygon number, vi - vertex number*/
void set_xyz(float *verts, int n, int i, int vi,
    float x, float y, float z)
{
  float *vertex;
  assert(n == 2 || n == 3 || n == 4);
  assert(vi >= 0 && vi <= n);
  assert(i >= 0);
  vertex = verts + (i * n * 3) + (3 * vi);
  *(vertex + 0) = x;
  *(vertex + 1) = y;
  *(vertex + 2) = z;
}

void set_rgb(GLubyte *colors, int n, int i, int vi,
    GLubyte r, GLubyte g, GLubyte b)
{
  GLubyte *color;
  assert(n == 2 || n == 3 || n == 4);
  assert(vi >= 0 && vi <= n);
  assert(i >= 0);
  color = colors + (i * n * 3) + (3 * vi);
  color[0] = r;
  color[1] = g;
  color[2] = b;
}

void set_rgb_i(GLubyte *colors, int n, int i, int vi,
    int r, int g, int b)
{
  set_rgb(colors, n, i, vi,
    (GLubyte)r, (GLubyte)g, (GLubyte)b);
}
