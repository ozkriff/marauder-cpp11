// See LICENSE file for copyright and license details.

#include "visualizer/meshBuilders.hpp"
#include "visualizer/visualizer.hpp"
#include "visualizer/math.hpp"

Mesh buildWalkableArray(Map &map) {
  Mesh v(Color(0.0f, 0.0f, 1.0f), PrimitiveType::Lines);
  map.forEachPosition([&](const V2i& p) {
    const Tile& t = map.tile(p);
    if (t.parent.value() != DirectionID::NONE && t.cost < 50) {
      V2i to = Direction::getNeighbourPosition(p, t.parent);
      if (map.isInboard(to)) {
        V2f fromF = v2iToV2f(p);
        V2f toF = v2iToV2f(to);
        v.addVertex(V3f(fromF, 0.01f));
        v.addVertex(V3f(toF, 0.01f));
      }
    }
  });
  return v;
}

Color3u fowColor(const Map& map, const V2i& position) {
  GLuint n;
  if (map.tile(position).fow == 0) {
    n = 180; // dark
  } else {
    n = 255; // bright
  }
  return Color3u(n);
}

Mesh buildMapArray(Map &map, GLuint textureID) {
  Mesh v;
  v.setTextureID(textureID);
  map.forEachPosition([&](const V2i& p) {
    V2f position = v2iToV2f(p);
    Color3u color = fowColor(map, p);
    for (int i = 0; i < 6; ++i) {
      v.addVertex(position + indexToHexVertex(i), V2f(0.0f, 0.0f), color);
      v.addVertex(position + indexToHexVertex(i + 1), V2f(1.0f, 0.0f), color);
      v.addVertex(position, V2f(0.5f, 0.5f), color);
    }
  });
  return v;
}

Mesh buildObstaclesArray(Map& map, GLuint textureID) {
  Mesh v(Color(0.4f, 0.1f, 0.0f));
  v.setTextureID(textureID);
  map.forEachPosition([&](const V2i& p) {
    if (map.tile(p).obstacle) {
      V2f position = v2iToV2f(p);
      for (int i = 0; i < 6; ++i) {
        v.addVertex(
            V3f(position + indexToHexVertex(i) * 0.7f, 0.01f),
            V2f(0.0f, 0.0f));
        v.addVertex(
            V3f(position + indexToHexVertex(i + 1) * 0.7f, 0.01f),
            V2f(1.0f, 0.0f));
        v.addVertex(
            V3f(position, 0.01f),
            V2f(0.5f, 0.5f));
      }
    }
  });
  return v;
}

Mesh buildUnitCircleMesh(float radius, const Color& color)
{
  Mesh v(color, PrimitiveType::Lines);
  const int verticesCount = 12;
  for (int i = 0; i < verticesCount; ++i) {
    const float k = radius * 2.0f; // resize coefficient
    const float h = 0.01f;
    v.addVertex(V3f(
        indexToCircleVertex(verticesCount, i) * k, h));
    v.addVertex(V3f(
        indexToCircleVertex(verticesCount, i + 1) * k, h));
  }
  return v;
}

Mesh buildPickingTilesArray(Map& map) {
  Mesh v;
  map.forEachPosition([&](const V2i& p) {
    Color3u color(p.x(), p.y(), 1);
    V2f position = v2iToV2f(p);
    for (int i = 0; i < 6; ++i) {
      v.addVertex(V3f(position + indexToHexVertex(i)), color);
      v.addVertex(V3f(position + indexToHexVertex(i + 1)), color);
      v.addVertex(V3f(position), color);
    }
  });
  return v;
}
