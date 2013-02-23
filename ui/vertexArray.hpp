// See LICENSE file for copyright and license details.

#ifndef UI__VERTEX_ARRAY_HPP
#define UI__VERTEX_ARRAY_HPP

#include <vector>
#include "SDL_opengl.h"
#include "ui/v2f.hpp"
#include "ui/v3f.hpp"
#include "ui/color.hpp"

enum class PrimitiveType {
  Triangles,
  Lines
};

class VertexArray {
public:
  VertexArray(PrimitiveType type = PrimitiveType::Triangles);
  VertexArray(const Color& color, PrimitiveType type = PrimitiveType::Triangles);

  void draw();

  // TODO: make private
  std::vector<float> vertices;
  std::vector<float> textureCoordinates;
  std::vector<GLubyte> colors;

  PrimitiveType mPrimitiveType;
  GLuint mTextureID;
  Color mColor;
  bool mHaveColor;
};

void appendV2f(std::vector<float>* vertices, const V2f& vertex);
void appendV3f(std::vector<float>* vertices, const V3f& vertex);
void appendRGB(std::vector<GLubyte>* colors, GLubyte r, GLubyte g, GLubyte b);
void appendRGB(std::vector<GLubyte>* colors, int r, int g, int b);

#endif
