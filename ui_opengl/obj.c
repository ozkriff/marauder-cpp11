/* See LICENSE file for copyright and license details. */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <GL/gl.h>
#include "core/list.h"
#include "core/v2i.h"
#include "core/dir.h"
#include "core/misc.h"
#include "ui_opengl/v2f.h"
#include "ui_opengl/v3f.h"
#include "ui_opengl/math.h"
#include "ui_opengl/vertex_array.h"
#include "ui_opengl/obj.h"

/* TODO "usemtl filename" */
void obj_read(ObjModel *m, const char *filename) {
  char buffer[100];
  FILE *file;
  int v_i = 0;
  int n_i = 0;
  int t_i = 0;
  int f_i = 0;
  assert(m);
  assert(filename);
  m->f_count = 0;
  m->v_count = 0;
  m->t_count = 0;
  m->n_count = 0;
  file = fopen(filename, "r");
  if (!file) {
    die("obj_read(): can't find file: %s", filename);
  }
  while (fgets(buffer, 100, file)) {
    if (buffer[0] == 'v' && buffer[1] == ' ') {
      m->v_count++;
    } else if (buffer[0] == 'v' && buffer[1] == 'n') {
      m->n_count++;
    } else if (buffer[0] == 'v' && buffer[1] == 't') {
      m->t_count++;
    } else if (buffer[0] == 'f' && buffer[1] == ' ') {
      m->f_count++;
    }
  }
  rewind(file);
  m->vertexes = ALLOCATE(m->v_count, V3f);
  m->normals = ALLOCATE(m->n_count, V3f);
  m->text_coords = ALLOCATE(m->t_count, V2f);
  m->faces = ALLOCATE(m->f_count, ObjTriangle);
  while (fgets(buffer, 100, file)) {
    if (buffer[0] == 'v' && buffer[1] == ' ') {
      /* Vertex coords */
      int items;
      V3f *v;
      v = m->vertexes + v_i;
      items = sscanf(buffer, "v %f %f %f",
          &v->x, &v->y, &v->z);
      if (items != 3) {
        die("obj_read(): vertex coords: items != 3\n");
      }
      v_i++;
    } else if (buffer[0] == 'v' && buffer[1] == 'n') {
      /* Vertex normals */
      int items;
      V3f *norm = m->normals + n_i;
      items = sscanf(buffer, "vn %f %f %f",
          &norm->x, &norm->y, &norm->z);
      if (items != 3) {
        die("obj_read(): vertex normals: items != 3\n");
      }
      n_i++;
    } else if (buffer[0] == 'v' && buffer[1] == 't') {
      /* Texture coords */
      int items;
      V2f *tex = m->text_coords + t_i;
      items = sscanf(buffer, "vt %f %f", &tex->x, &tex->y);
      tex->y = 1.0f - tex->y; /* flip vertically */
      if (items != 2) {
        die("obj_read(): texture coords: items != 2\n");
      }
      t_i++;
    } else if (buffer[0] == 'f' && buffer [1] == ' ') {
      /* Faces */
      int items;
      ObjTriangle *t;
      int slash_count = 0;
      int i;
      for (i = 2; buffer[i] != ' '; i++)
        if (buffer[i] == '/') {
          slash_count++;
        }
      t = m->faces + f_i;
      if (slash_count == 1) {
        items = sscanf(buffer, "f %d/%d %d/%d %d/%d",
            &t->v[0], &t->t[0], &t->v[1],
            &t->t[1], &t->v[2], &t->t[2]);
        if (items != 6) {
          die("obj_read(): faces: items != 6\n");
        }
      } else if (slash_count == 2) {
        items = sscanf(buffer, "f %d/%d/%d %d/%d/%d %d/%d/%d",
            &t->v[0], &t->t[0], &t->n[0],
            &t->v[1], &t->t[1], &t->n[1],
            &t->v[2], &t->t[2], &t->n[2]);
        if (items != 9) {
          die("obj_read(): faces: items != 9\n");
        }
      } else {
        die("obj_read(): faces: bad format\n");
      }
      f_i++;
    }
  }
  fclose(file);
}

void obj_debug_print(ObjModel *m) {
  int i;
  for (i = 0; i < m->v_count; i++) {
    V3f *v = m->vertexes + i;
    printf("v %f %f %f\n", v->x, v->y, v->z);
  }
  for (i = 0; i < m->t_count; i++) {
    V2f *v = m->text_coords + i;
    printf("t %f %f\n", v->x, v->y);
  }
  for (i = 0; i < m->f_count; i++) {
    ObjTriangle *f = m->faces + i;
    printf("f %d/%d/%d %d/%d/%d %d/%d/%d\n",
        f->v[0], f->t[0], f->n[0],
        f->v[1], f->t[1], f->n[1],
        f->v[2], f->t[2], f->n[2]);
  }
}

void obj_build(VertexArray *va, const ObjModel *model) {
  int i, j;
  assert(va);
  assert(model);
  va->count = model->f_count * 3;
  va->v = ALLOCATE(va->count, V3f);
  va->t = ALLOCATE(va->count, V2f);
  for (i = 0; i < model->f_count; i++) {
    ObjTriangle *tri = model->faces + i;
    for (j = 0; j < 3; j++) {
      int vert_id = tri->v[j] - 1;
      int tex_id = tri->t[j] - 1;
      V3f *vert = model->vertexes + vert_id;
      V2f *tex = model->text_coords + tex_id;
      set_xyz(va->v, 3, i, j, vert->x, vert->y, vert->z);
      set_xy(va->t, 3, i, j, tex->x, tex->y);
    }
  }
}
