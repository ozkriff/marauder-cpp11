// See LICENSE file for copyright and license details.

#ifndef UI__OBJ_MODEL_HPP
#define UI__OBJ_MODEL_HPP

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

  void read(const std::string& filename);

public:
  ObjModel(const std::string& filename);

  VertexArray build();
  void debugPrint();
};

#endif