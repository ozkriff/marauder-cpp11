// See LICENSE file for copyright and license details.

#include <cassert>
#include <stdexcept>
#include "visualizer/vertexArray.hpp"

VertexArray::VertexArray(PrimitiveType primitiveType)
    : vertices(),
      textureCoordinates(),
      colors(),
      mPrimitiveType(primitiveType),
      mTextureID(0),
      mColor(1.0f, 1.0f, 1.0f),
      mHaveColor(false)
{
}

VertexArray::VertexArray(const Color& color, PrimitiveType primitiveType)
    : vertices(),
      textureCoordinates(),
      colors(),
      mPrimitiveType(primitiveType),
      mTextureID(0),
      mColor(color),
      mHaveColor(true)
{
}

void VertexArray::draw() {
  // enable everything
  {
    if (!textureCoordinates.empty()) {
      glEnable(GL_TEXTURE_2D);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glBindTexture(GL_TEXTURE_2D, mTextureID);
      glTexCoordPointer(2, GL_FLOAT, 0, textureCoordinates.data());
    }
    if (!vertices.empty()) {
      glEnableClientState(GL_VERTEX_ARRAY);
    }
    if (!colors.empty()) {
      glEnableClientState(GL_COLOR_ARRAY);
      glColorPointer(3, GL_UNSIGNED_BYTE, 0, colors.data());
    }
    if (mHaveColor) {
      glColor3f(mColor.red(), mColor.green(), mColor.blue());
    } else {
      glColor3f(1.0f, 1.0f, 1.0f);
    }
  }
  // draw
  {
    glVertexPointer(3, GL_FLOAT, 0, vertices.data());
    switch (mPrimitiveType) {
    case PrimitiveType::Triangles:
      glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);
      break;
    case PrimitiveType::Lines:
      glDrawArrays(GL_LINES, 0, vertices.size() / 3);
      break;
    default:
      throw std::logic_error("default case");
    }
  }
  // disable everything
  {
    if (!textureCoordinates.empty()) {
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      glDisable(GL_TEXTURE_2D);
    }
    if (!vertices.empty()) {
      glDisableClientState(GL_VERTEX_ARRAY);
    }
    if (!colors.empty()) {
      glDisableClientState(GL_COLOR_ARRAY);
    }
  }
}

void appendV2f(std::vector<float>* vertices, const V2f& vertex) {
  assert(vertices);
  vertices->push_back(vertex.x());
  vertices->push_back(vertex.y());
}

void appendV3f(std::vector<float>* vertices, const V3f& vertex) {
  assert(vertices);
  vertices->push_back(vertex.x());
  vertices->push_back(vertex.y());
  vertices->push_back(vertex.z());
}

void appendRGB(std::vector<GLubyte>* colors, GLubyte r, GLubyte g, GLubyte b) {
  assert(colors);
  colors->push_back(r);
  colors->push_back(g);
  colors->push_back(b);
}

void appendRGB(std::vector<GLubyte>* colors, int r, int g, int b) {
  assert(r >= 0 && r < 256);
  assert(g >= 0 && g < 256);
  assert(b >= 0 && b < 256);
  assert(colors);
  appendRGB(colors,
    static_cast<GLubyte>(r),
    static_cast<GLubyte>(g),
    static_cast<GLubyte>(b));
}
