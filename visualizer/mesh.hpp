// See LICENSE file for copyright and license details.

#ifndef VISUALIZER__MESH_HPP
#define VISUALIZER__MESH_HPP

#include <vector>
#include "visualizer/texture.hpp"
#include "visualizer/v2f.hpp"
#include "visualizer/v3f.hpp"
#include "visualizer/color4f.hpp"
#include "visualizer/color3u.hpp"

enum class PrimitiveType {
  Triangles,
  Lines
};

class Mesh {
public:
  Mesh(PrimitiveType type = PrimitiveType::Triangles);
  Mesh(const Color4f& color, PrimitiveType type = PrimitiveType::Triangles);

  void setTextureID(UnsignedInteger textureID);

  void addVertex(
      const V3f& meshCoordinate,
      const V2f& textureCoordinate,
      const Color3u& color);
  void addVertex(const V3f& meshCoordinate, const V2f& textureCoordinate);
  void addVertex(const V3f& meshCoordinate, const Color3u& color);
  void addVertex(const V3f& meshCoordinate);

  void draw();

private:
  std::vector<float> mVertices;
  std::vector<float> mTextureCoordinates;
  std::vector<UnsignedByte> mColors;
  UnsignedInteger mTextureID;
  PrimitiveType mPrimitiveType;
  Color4f mColor;
  bool mHaveColor;

  void addColor(const Color3u& color);
  void addTextureCoord(const V2f& vertex);
  void addCoord(const V3f& vertex);
};

#endif
