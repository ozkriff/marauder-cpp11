// See LICENSE file for copyright and license details.

#ifndef VISUALIZER__VERTEX_ARRAY_HPP
#define VISUALIZER__VERTEX_ARRAY_HPP

#include <vector>
#include "SDL_opengl.h"
#include "visualizer/v2f.hpp"
#include "visualizer/v3f.hpp"
#include "visualizer/color.hpp"
#include "visualizer/color3u.hpp"

enum class PrimitiveType {
  Triangles,
  Lines
};

class VertexArray {
public:
  VertexArray(PrimitiveType type = PrimitiveType::Triangles);
  VertexArray(const Color& color, PrimitiveType type = PrimitiveType::Triangles);

  void setTextureID(GLuint textureID);

  void addColor(const Color3u& color);
  void addTextureCoord(const V2f& vertex);
  void addCoord(const V3f& vertex);

  void draw();

private:
  std::vector<float> mVertices;
  std::vector<float> mTextureCoordinates;
  std::vector<GLubyte> mColors;
  GLuint mTextureID;
  PrimitiveType mPrimitiveType;
  Color mColor;
  bool mHaveColor;
};

#endif
