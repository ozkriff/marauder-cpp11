// See LICENSE file for copyright and license details.

#include <cassert>
#include <SDL/SDL_opengl.h>
#include "ui_opengl/vertex_array.h"

const VertexArray emptyVertexArray = {nullptr, nullptr, nullptr, 0};

void setXY(float *coords, int n, int i, int vi,
    float x, float y)
{
  assert(n == 2 || n == 3 || n == 4);
  assert(vi >= 0 && vi <= n);
  assert(i >= 0);
  assert(coords);
  float* coord = coords + (i * n * 2) + (2 * vi);
  *(coord + 0) = x;
  *(coord + 1) = y;
}

/*if(quads) n=4; if(tris) n=3*/
/* i - polygon number, vi - vertex number*/
void setXYZ(float *verts, int n, int i, int vi,
    float x, float y, float z)
{
  assert(n == 2 || n == 3 || n == 4);
  assert(vi >= 0 && vi <= n);
  assert(i >= 0);
  assert(verts);
  float* vertex = verts + (i * n * 3) + (3 * vi);
  *(vertex + 0) = x;
  *(vertex + 1) = y;
  *(vertex + 2) = z;
}

void setRGB(GLubyte *colors, int n, int i, int vi,
    GLubyte r, GLubyte g, GLubyte b)
{
  assert(n == 2 || n == 3 || n == 4);
  assert(vi >= 0 && vi <= n);
  assert(i >= 0);
  assert(colors);
  GLubyte* color = colors + (i * n * 3) + (3 * vi);
  color[0] = r;
  color[1] = g;
  color[2] = b;
}

void setRGBi(GLubyte *colors, int n, int i, int vi,
    int r, int g, int b)
{
  assert(r >= 0 && r < 256);
  assert(g >= 0 && g < 256);
  assert(b >= 0 && b < 256);
  setRGB(colors, n, i, vi,
    (GLubyte)r, (GLubyte)g, (GLubyte)b);
}
