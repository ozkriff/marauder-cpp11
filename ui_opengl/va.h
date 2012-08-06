/* See LICENSE file for copyright and license details. */

/* Vertex array */
typedef struct {
  float *v; /* vertices */
  float *t; /* [opt] texture coordinates */
  GLubyte *ub_c; /* [opt] colors */
  int count; /* vertices count */
} Va;

extern const Va empty_va;

void set_xy(float *coords, int n, int i, int vi,
    float x, float y);
void set_xyz(float *verts, int n, int i, int vi,
    float x, float y, float z);
void set_rgb(GLubyte *colors, int n, int i, int vi,
    GLubyte r, GLubyte g, GLubyte b);
void set_rgb_i(GLubyte *colors, int n, int i, int vi,
    int r, int g, int b);
