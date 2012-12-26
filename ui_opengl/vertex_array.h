/* See LICENSE file for copyright and license details. */

#ifndef UI_OPENGL_VERTEX_ARRAY_H
#define UI_OPENGL_VERTEX_ARRAY_H

#include <SDL/SDL_opengl.h>

/* Vertex array */
struct VertexArray {
  float *v; /* vertices */
  float *t; /* [opt] texture coordinates */
  GLubyte *ub_c; /* [opt] colors */
  int count; /* vertices count */
};

extern const VertexArray empty_vertex_array;

void set_xy(float *coords, int n, int i, int vi,
    float x, float y);
void set_xyz(float *verts, int n, int i, int vi,
    float x, float y, float z);
void set_rgb(GLubyte *colors, int n, int i, int vi,
    GLubyte r, GLubyte g, GLubyte b);
void set_rgb_i(GLubyte *colors, int n, int i, int vi,
    int r, int g, int b);

#endif
