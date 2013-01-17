// See LICENSE file for copyright and license details.

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <GL/gl.h>
#include "core/dir.hpp"
#include "core/misc.hpp"
#include "ui/math.hpp"
#include "ui/vertex_array.hpp"
#include "ui/obj.hpp"

ObjModel::ObjModel(const std::string& filename) {
  read(filename);
}

// TODO "usemtl filename"
void ObjModel::read(const std::string& filename) {
  auto file = fopen(filename.c_str(), "r");
  if (!file) {
    die("objRead(): can't find file: %s", filename.c_str());
  }
  char buffer[100];
  while (fgets(buffer, 100, file)) {
    if (buffer[0] == 'v' && buffer[1] == ' ') {
      // Vertex coords
      V3f v;
      int items = sscanf(buffer, "v %f %f %f",
          &v.x, &v.y, &v.z);
      if (items != 3) {
        die("objRead(): vertex coords: items != 3\n");
      }
      mVertices.push_back(v);
    } else if (buffer[0] == 'v' && buffer[1] == 'n') {
      // Vertex normals
      V3f norm;
      int items = sscanf(buffer, "vn %f %f %f",
          &norm.x, &norm.y, &norm.z);
      if (items != 3) {
        die("objRead(): vertex normals: items != 3\n");
      }
      mNormals.push_back(norm);
    } else if (buffer[0] == 'v' && buffer[1] == 't') {
      // Texture coords
      float x, y;
      int items = sscanf(buffer, "vt %f %f", &x, &y);
      y = 1.0f - y; // flip vertically
      if (items != 2) {
        die("objRead(): texture coords: items != 2\n");
      }
      mTextureCoords.push_back(V2f(x, y));
    } else if (buffer[0] == 'f' && buffer [1] == ' ') {
      // Faces
      ObjTriangle t;
      int items = sscanf(buffer, "f %d/%d/%d %d/%d/%d %d/%d/%d",
          &t.vertex[0], &t.texture[0], &t.normal[0],
          &t.vertex[1], &t.texture[1], &t.normal[1],
          &t.vertex[2], &t.texture[2], &t.normal[2]);
      if (items != 9) {
        die("objRead(): faces: items != 9\n");
      }
      mFaces.push_back(t);
    }
  }
  fclose(file);
}

void ObjModel::debugPrint() {
  for (auto v : mVertices) {
    printf("v %f %f %f\n", v.x, v.y, v.z);
  }
  for (auto v : mTextureCoords) {
    printf("t %f %f\n", v.x(), v.y());
  }
  for (auto f : mFaces) {
    printf("f %d/%d/%d %d/%d/%d %d/%d/%d\n",
        f.vertex[0], f.texture[0], f.normal[0],
        f.vertex[1], f.texture[1], f.normal[1],
        f.vertex[2], f.texture[2], f.normal[2]);
  }
}

VertexArray ObjModel::build() {
  VertexArray va;
  for (unsigned int i = 0; i < mFaces.size(); i++) {
    auto tri = mFaces[i];
    for (int j = 0; j < 3; j++) {
      int vertexID = tri.vertex[j] - 1;
      int textureCoordID = tri.texture[j] - 1;
      appendV3f(&va.vertices, mVertices[vertexID]);
      appendV2f(&va.textureCoordinates, mTextureCoords[textureCoordID]);
    }
  }
  return va;
}
