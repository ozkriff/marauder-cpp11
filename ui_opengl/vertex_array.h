// See LICENSE file for copyright and license details.

#ifndef UI_OPENGL_VERTEX_ARRAY_H
#define UI_OPENGL_VERTEX_ARRAY_H

#include <SDL/SDL_opengl.h>

struct VertexArray {
  float* v; // vertices
  float* t; // [opt] texture coordinates
  GLubyte* ubC; // [opt] colors
  int count; // vertices count
};

extern const VertexArray emptyVertexArray;

void setXY(float* coords, int n, int i, int vi,
    float x, float y);
void setXYZ(float* verts, int n, int i, int vi,
    float x, float y, float z);
void setRGB(GLubyte* colors, int n, int i, int vi,
    GLubyte r, GLubyte g, GLubyte b);
void setRGBi(GLubyte* colors, int n, int i, int vi,
    int r, int g, int b);

#endif
