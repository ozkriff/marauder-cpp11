// See LICENSE file for copyright and license details.

#ifndef VISUALIZER__VERTEX_ARRAY_HPP
#define VISUALIZER__VERTEX_ARRAY_HPP

#include <vector>
#include "SDL_opengl.h"
#include "visualizer/v2f.hpp"
#include "visualizer/v3f.hpp"
#include "visualizer/color.hpp"

enum class PrimitiveType {
  Triangles,
  Lines
};

class VertexArray {
public:
  VertexArray(PrimitiveType type = PrimitiveType::Triangles);
  VertexArray(const Color& color, PrimitiveType type = PrimitiveType::Triangles);

  void setTextureID(GLuint textureID);

  void draw();

  // TODO: make private
  std::vector<float> vertices;
  std::vector<float> textureCoordinates;
  std::vector<GLubyte> colors;

private:
  GLuint mTextureID;
  PrimitiveType mPrimitiveType;
  Color mColor;
  bool mHaveColor;
};

void appendV2f(std::vector<float>* vertices, const V2f& vertex);
void appendV3f(std::vector<float>* vertices, const V3f& vertex);
void appendRGB(std::vector<GLubyte>* colors, GLubyte r, GLubyte g, GLubyte b);
void appendRGB(std::vector<GLubyte>* colors, int r, int g, int b);

#endif
