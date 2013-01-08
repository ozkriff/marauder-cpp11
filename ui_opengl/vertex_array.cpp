// See LICENSE file for copyright and license details.

#include <cassert>
#include "ui_opengl/vertex_array.hpp"

void appendV2f(std::vector<float>* vertices, const V2f& vertex) {
  assert(vertices);
  vertices->push_back(vertex.x());
  vertices->push_back(vertex.y());
}

void appendV3f(std::vector<float>* vertices, const V3f& vertex) {
  assert(vertices);
  vertices->push_back(vertex.x);
  vertices->push_back(vertex.y);
  vertices->push_back(vertex.z);
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