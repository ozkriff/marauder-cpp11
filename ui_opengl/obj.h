// See LICENSE file for copyright and license details.

#ifndef UI_OPENGL_OBJ_H
#define UI_OPENGL_OBJ_H

#include "ui_opengl/v3f.h"

// vertex, texture, normal
struct ObjTriangle {
  int v[3];
  int t[3];
  int n[3];
};

struct ObjModel {
  V3f* vertexes;
  V3f* normals;
  V2f* textCoords;
  ObjTriangle* faces;
  int fCount;
  int vCount;
  int tCount;
  int nCount;
};

void objBuild(VertexArray* va, const ObjModel* model);
void objRead(ObjModel* m, const char* filename);
void objDebugPrint(ObjModel* m);

#endif
