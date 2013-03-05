// See LICENSE file for copyright and license details.

#include <cassert>
#include <stdexcept>
#include "visualizer/vertexArray.hpp"

VertexArray::VertexArray(PrimitiveType primitiveType)
    : mVertices(),
      mTextureCoordinates(),
      mColors(),
      mTextureID(0),
      mPrimitiveType(primitiveType),
      mColor(1.0f, 1.0f, 1.0f),
      mHaveColor(false)
{
}

VertexArray::VertexArray(const Color& color, PrimitiveType primitiveType)
    : mVertices(),
      mTextureCoordinates(),
      mColors(),
      mTextureID(0),
      mPrimitiveType(primitiveType),
      mColor(color),
      mHaveColor(true)
{
}

void VertexArray::setTextureID(GLuint textureID) {
  mTextureID = textureID;
}

void VertexArray::addColorRGB(int r, int g, int b) {
  assert(r >= 0 && r < 256);
  assert(g >= 0 && g < 256);
  assert(b >= 0 && b < 256);
  addColorRGB(
      static_cast<GLubyte>(r),
      static_cast<GLubyte>(g),
      static_cast<GLubyte>(b));
}

void VertexArray::addColorRGB(GLubyte r, GLubyte g, GLubyte b) {
  mColors.push_back(r);
  mColors.push_back(g);
  mColors.push_back(b);
}

void VertexArray::addTextureCoord(const V2f& vertex) {
  mTextureCoordinates.push_back(vertex.x());
  mTextureCoordinates.push_back(vertex.y());
}

void VertexArray::addCoord(const V3f& vertex) {
  mVertices.push_back(vertex.x());
  mVertices.push_back(vertex.y());
  mVertices.push_back(vertex.z());
}

void VertexArray::draw() {
  // enable everything
  {
    if (!mTextureCoordinates.empty()) {
      glEnable(GL_TEXTURE_2D);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glBindTexture(GL_TEXTURE_2D, mTextureID);
      glTexCoordPointer(2, GL_FLOAT, 0, mTextureCoordinates.data());
    }
    if (!mVertices.empty()) {
      glEnableClientState(GL_VERTEX_ARRAY);
    }
    if (!mColors.empty()) {
      glEnableClientState(GL_COLOR_ARRAY);
      glColorPointer(3, GL_UNSIGNED_BYTE, 0, mColors.data());
    }
    if (mHaveColor) {
      glColor3f(mColor.red(), mColor.green(), mColor.blue());
    } else {
      glColor3f(1.0f, 1.0f, 1.0f);
    }
  }
  // draw
  {
    glVertexPointer(3, GL_FLOAT, 0, mVertices.data());
    switch (mPrimitiveType) {
    case PrimitiveType::Triangles:
      glDrawArrays(GL_TRIANGLES, 0, mVertices.size() / 3);
      break;
    case PrimitiveType::Lines:
      glDrawArrays(GL_LINES, 0, mVertices.size() / 3);
      break;
    default:
      throw std::logic_error("default case");
    }
  }
  // disable everything
  {
    if (!mTextureCoordinates.empty()) {
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      glDisable(GL_TEXTURE_2D);
    }
    if (!mVertices.empty()) {
      glDisableClientState(GL_VERTEX_ARRAY);
    }
    if (!mColors.empty()) {
      glDisableClientState(GL_COLOR_ARRAY);
    }
  }
}

void appendV3f(std::vector<float>* vertices, const V3f& vertex) {
  assert(vertices);
  vertices->push_back(vertex.x());
  vertices->push_back(vertex.y());
  vertices->push_back(vertex.z());
}
