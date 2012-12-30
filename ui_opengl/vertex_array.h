// See LICENSE file for copyright and license details.

#ifndef UI_OPENGL_VERTEX_ARRAY_H
#define UI_OPENGL_VERTEX_ARRAY_H

#include <vector>
#include <SDL/SDL_opengl.h>
#include "ui_opengl/v2f.h"
#include "ui_opengl/v3f.h"

struct VertexArray {
  std::vector<float> vertices;
  std::vector<float> textureCoordinates;
  std::vector<GLubyte> colors;
};

void appendV2f(std::vector<float>* vertices, const V2f& vertex);
void appendV3f(std::vector<float>* vertices, const V3f& vertex);
void appendRGB(std::vector<GLubyte>* colors, GLubyte r, GLubyte g, GLubyte b);
void appendRGB(std::vector<GLubyte>* colors, int r, int g, int b);

#endif
