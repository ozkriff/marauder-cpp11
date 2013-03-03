// See LICENSE file for copyright and license details.

#include "visualizer/objModel.hpp"
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <stdexcept>
#include "visualizer/math.hpp"

ObjModel::ObjModel(const std::string& filename) {
  read(filename);
}

namespace {

V3f readVertexCoord(const char* buffer) {
  float x, y, z;
  int items = std::sscanf(buffer, "v %f %f %f", &x, &y, &z);
  if (items != 3) {
    throw std::runtime_error("items != 3");
  }
  return V3f(x, y, z);
}

V3f readVertexNormal(const char* buffer) {
  float x, y, z;
  int items = std::sscanf(buffer, "vn %f %f %f", &x, &y, &z);
  if (items != 3) {
    throw std::runtime_error("items != 3");
  }
  return V3f(x, y, z);
}

V2f readTextureCoords(const char* buffer) {
  float x, y;
  int items = std::sscanf(buffer, "vt %f %f", &x, &y);
  if (items != 2) {
    throw std::runtime_error("items != 3");
  }
  y = 1.0f - y; // flip vertically
  return V2f(x, y);
}

ObjModel::ObjTriangle readFace(const char* buffer) {
  ObjModel::ObjTriangle t;
  int items = std::sscanf(buffer, "f %d/%d/%d %d/%d/%d %d/%d/%d",
      &t.vertex[0], &t.texture[0], &t.normal[0],
      &t.vertex[1], &t.texture[1], &t.normal[1],
      &t.vertex[2], &t.texture[2], &t.normal[2]);
  if (items != 9) {
    throw std::runtime_error("items != 9");
  }
  return t;
}

} // namespace

// TODO "usemtl filename"
void ObjModel::read(const std::string& filename) {
  auto* file = std::fopen(filename.c_str(), "r");
  if (!file) {
    throw std::runtime_error(std::string("can't find file: ") + filename);
  }
  char buffer[100];
  while (std::fgets(buffer, 100, file)) {
    if (buffer[0] == 'v' && buffer[1] == ' ') {
      mVertices.push_back(readVertexCoord(buffer));
    } else if (buffer[0] == 'v' && buffer[1] == 'n') {
      mVertices.push_back(readVertexNormal(buffer));
    } else if (buffer[0] == 'v' && buffer[1] == 't') {
      mTextureCoords.push_back(readTextureCoords(buffer));
    } else if (buffer[0] == 'f' && buffer[1] == ' ') {
      mFaces.push_back(readFace(buffer));
    }
  }
  std::fclose(file);
}

void ObjModel::debugPrint() {
  for (auto v : mVertices) {
    std::printf("v %f %f %f\n", v.x(), v.y(), v.z());
  }
  for (auto v : mTextureCoords) {
    std::printf("t %f %f\n", v.x(), v.y());
  }
  for (auto f : mFaces) {
    std::printf("f %d/%d/%d %d/%d/%d %d/%d/%d\n",
        f.vertex[0], f.texture[0], f.normal[0],
        f.vertex[1], f.texture[1], f.normal[1],
        f.vertex[2], f.texture[2], f.normal[2]);
  }
}

VertexArray ObjModel::build() {
  VertexArray va(PrimitiveType::Triangles);
  for (unsigned int i = 0; i < mFaces.size(); ++i) {
    auto tri = mFaces[i];
    for (int j = 0; j < 3; ++j) {
      int vertexID = tri.vertex[j] - 1;
      int textureCoordID = tri.texture[j] - 1;
      appendV3f(&va.vertices, mVertices[vertexID]);
      appendV2f(&va.textureCoordinates, mTextureCoords[textureCoordID]);
    }
  }
  return va;
}
