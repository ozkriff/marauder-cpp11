/* See LICENSE file for copyright and license details. */

#ifndef UI_OPENGL_OBJ_H
#define UI_OPENGL_OBJ_H

/* vertex, texture, normal */
struct ObjTriangle {
  int v[3];
  int t[3];
  int n[3];
};

struct ObjModel {
  V3f *vertexes;
  V3f *normals;
  V2f *text_coords;
  ObjTriangle *faces;
  int f_count;
  int v_count;
  int t_count;
  int n_count;
};

void obj_build(VertexArray *va, const ObjModel *model);
void obj_read(ObjModel *m, const char *filename);
void obj_debug_print(ObjModel *m);

#endif
