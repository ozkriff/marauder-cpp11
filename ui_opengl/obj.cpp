// See LICENSE file for copyright and license details.

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <GL/gl.h>
#include "core/dir.h"
#include "core/misc.h"
#include "ui_opengl/math.h"
#include "ui_opengl/vertex_array.h"
#include "ui_opengl/obj.h"

// TODO "usemtl filename"
void objRead(ObjModel* m, const char* filename) {
  char buffer[100];
  FILE *file;
  int vI = 0;
  int nI = 0;
  int tI = 0;
  int fI = 0;
  assert(m);
  assert(filename);
  m->fCount = 0;
  m->vCount = 0;
  m->tCount = 0;
  m->nCount = 0;
  file = fopen(filename, "r");
  if (!file) {
    die("objRead(): can't find file: %s", filename);
  }
  while (fgets(buffer, 100, file)) {
    if (buffer[0] == 'v' && buffer[1] == ' ') {
      m->vCount++;
    } else if (buffer[0] == 'v' && buffer[1] == 'n') {
      m->nCount++;
    } else if (buffer[0] == 'v' && buffer[1] == 't') {
      m->tCount++;
    } else if (buffer[0] == 'f' && buffer[1] == ' ') {
      m->fCount++;
    }
  }
  rewind(file);
  m->vertexes = new V3f[m->vCount];
  m->normals = new V3f[m->nCount];
  m->textCoords = new V2f[m->tCount];
  m->faces = new ObjTriangle[m->fCount];
  while (fgets(buffer, 100, file)) {
    if (buffer[0] == 'v' && buffer[1] == ' ') {
      // Vertex coords
      int items;
      V3f *v;
      v = m->vertexes + vI;
      items = sscanf(buffer, "v %f %f %f",
          &v->x, &v->y, &v->z);
      if (items != 3) {
        die("objRead(): vertex coords: items != 3\n");
      }
      vI++;
    } else if (buffer[0] == 'v' && buffer[1] == 'n') {
      // Vertex normals
      int items;
      V3f *norm = m->normals + nI;
      items = sscanf(buffer, "vn %f %f %f",
          &norm->x, &norm->y, &norm->z);
      if (items != 3) {
        die("objRead(): vertex normals: items != 3\n");
      }
      nI++;
    } else if (buffer[0] == 'v' && buffer[1] == 't') {
      // Texture coords
      int items;
      V2f *tex = m->textCoords + tI;
      float x, y;
      items = sscanf(buffer, "vt %f %f", &x, &y);
      *tex = V2f(x, y);
      tex->setY(1.0f - tex->y()); // flip vertically
      if (items != 2) {
        die("objRead(): texture coords: items != 2\n");
      }
      tI++;
    } else if (buffer[0] == 'f' && buffer [1] == ' ') {
      // Faces
      int items;
      ObjTriangle *t;
      int slashCount = 0;
      int i;
      for (i = 2; buffer[i] != ' '; i++)
        if (buffer[i] == '/') {
          slashCount++;
        }
      t = m->faces + fI;
      if (slashCount == 1) {
        items = sscanf(buffer, "f %d/%d %d/%d %d/%d",
            &t->v[0], &t->t[0], &t->v[1],
            &t->t[1], &t->v[2], &t->t[2]);
        if (items != 6) {
          die("objRead(): faces: items != 6\n");
        }
      } else if (slashCount == 2) {
        items = sscanf(buffer, "f %d/%d/%d %d/%d/%d %d/%d/%d",
            &t->v[0], &t->t[0], &t->n[0],
            &t->v[1], &t->t[1], &t->n[1],
            &t->v[2], &t->t[2], &t->n[2]);
        if (items != 9) {
          die("objRead(): faces: items != 9\n");
        }
      } else {
        die("objRead(): faces: bad format\n");
      }
      fI++;
    }
  }
  fclose(file);
}

void objDebugPrint(ObjModel *m) {
  int i;
  for (i = 0; i < m->vCount; i++) {
    V3f *v = m->vertexes + i;
    printf("v %f %f %f\n", v->x, v->y, v->z);
  }
  for (i = 0; i < m->tCount; i++) {
    V2f *v = m->textCoords + i;
    printf("t %f %f\n", v->x(), v->y());
  }
  for (i = 0; i < m->fCount; i++) {
    ObjTriangle *f = m->faces + i;
    printf("f %d/%d/%d %d/%d/%d %d/%d/%d\n",
        f->v[0], f->t[0], f->n[0],
        f->v[1], f->t[1], f->n[1],
        f->v[2], f->t[2], f->n[2]);
  }
}

void objBuild(VertexArray *va, const ObjModel *model) {
  int i, j;
  assert(va);
  assert(model);
  va->count = model->fCount * 3;
  va->v = (float *)new V3f[va->count];
  va->t = (float *)new V2f[va->count];
  for (i = 0; i < model->fCount; i++) {
    ObjTriangle *tri = model->faces + i;
    for (j = 0; j < 3; j++) {
      int vertId = tri->v[j] - 1;
      int texId = tri->t[j] - 1;
      V3f *vert = model->vertexes + vertId;
      V2f *tex = model->textCoords + texId;
      setXYZ(va->v, 3, i, j, vert->x, vert->y, vert->z);
      setXY(va->t, 3, i, j, tex->x(), tex->y());
    }
  }
}
