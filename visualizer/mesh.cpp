// See LICENSE file for copyright and license details.

#include <cassert>
#include <stdexcept>
#include "SDL_opengl.h"
#include "visualizer/mesh.hpp"

Mesh::Mesh(PrimitiveType primitiveType)
  : mVertices(),
    mTextureCoordinates(),
    mColors(),
    mTextureID(0),
    mPrimitiveType(primitiveType),
    mColor(1.0f, 1.0f, 1.0f),
    mHaveColor(false)
{
}

Mesh::Mesh(const Color4f& color, PrimitiveType primitiveType)
  : mVertices(),
    mTextureCoordinates(),
    mColors(),
    mTextureID(0),
    mPrimitiveType(primitiveType),
    mColor(color),
    mHaveColor(true)
{
}

void Mesh::setTextureID(UnsignedInteger textureID) {
  mTextureID = textureID;
}

void Mesh::addVertex(
    const V3f& meshCoordinate,
    const V2f& textureCoordinate,
    const Color3u& color)
{
  addCoord(meshCoordinate);
  addTextureCoord(textureCoordinate);
  addColor(color);
}

void Mesh::addVertex(
    const V3f& meshCoordinate, const V2f& textureCoordinate)
{
  addCoord(meshCoordinate);
  addTextureCoord(textureCoordinate);
}

void Mesh::addVertex(const V3f& meshCoordinate, const Color3u& color) {
  addCoord(meshCoordinate);
  addColor(color);
}

void Mesh::addVertex(const V3f& meshCoordinate) {
  addCoord(meshCoordinate);
}

void Mesh::addColor(const Color3u &color) {
  mColors.push_back(color.red());
  mColors.push_back(color.green());
  mColors.push_back(color.blue());
}

void Mesh::addTextureCoord(const V2f& vertex) {
  mTextureCoordinates.push_back(vertex.x());
  mTextureCoordinates.push_back(vertex.y());
}

void Mesh::addCoord(const V3f& vertex) {
  mVertices.push_back(vertex.x());
  mVertices.push_back(vertex.y());
  mVertices.push_back(vertex.z());
}

void Mesh::draw() {
  {
    assert(sizeof(UnsignedByte) == sizeof(GLubyte));
    assert(sizeof(UnsignedInteger) == sizeof(GLuint));
    assert(sizeof(float) == sizeof(GLfloat));
  }
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
