/* See LICENSE file for copyright and license details. */

/* vertex, texture, normal */
typedef struct {
  int v[3];
  int t[3];
  int n[3];
} ObjTriangle;

typedef struct {
  V3f *vertexes;
  V3f *normals;
  V2f *text_coords;
  ObjTriangle *faces;
  int f_count;
  int v_count;
  int t_count;
  int n_count;
} ObjModel;

void obj_build(Va *va, const ObjModel *model);
void obj_read(ObjModel *m, const char *filename);
void obj_debug_print(ObjModel *m);
