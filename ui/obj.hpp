// See LICENSE file for copyright and license details.

#ifndef UI_OPENGL_OBJ_H
#define UI_OPENGL_OBJ_H

#include <vector>
#include <string>
#include "ui/v3f.hpp"
#include "ui/v2f.hpp"

class ObjModel {
private:
  // indices
  struct ObjTriangle {
    int vertex[3];
    int texture[3];
    int normal[3];
  };

  std::vector<V3f> mVertices;
  std::vector<V3f> mNormals;
  std::vector<V2f> mTextureCoords;
  std::vector<ObjTriangle> mFaces;

public:
  VertexArray build();
  void read(const std::string& filename);
  void debugPrint();
};

#endif
