// See LICENSE file for copyright and license details.

#ifndef VISUALIZER__OBJ_MODEL_HPP
#define VISUALIZER__OBJ_MODEL_HPP

#include <vector>
#include <string>
#include "visualizer/v3f.hpp"
#include "visualizer/v2f.hpp"
#include "visualizer/mesh.hpp"

class ObjModel {
public:
  // indices
  class ObjTriangle {
  public:
    int vertex[3];
    int texture[3];
    int normal[3];
  };

  ObjModel(const std::string& filename);

  Mesh build();
  void debugPrint();

private:
  std::vector<V3f> mVertices;
  std::vector<V3f> mNormals;
  std::vector<V2f> mTextureCoords;
  std::vector<ObjTriangle> mFaces;

  void read(const std::string& filename);
};

#endif
