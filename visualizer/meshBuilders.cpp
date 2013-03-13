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
    float n = 0.5f;
    // first triangle
    v.addVertex(position + V2f(-n, -n), V2f(0.0f, 0.0f), color);
    v.addVertex(position + V2f(-n, n), V2f(0.0f, 1.0f), color);
    v.addVertex(position + V2f(n, n), V2f(1.0f, 1.0f), color);
    // second triangle
    v.addVertex(position + V2f(-n, -n), V2f(0.0f, 0.0f), color);
    v.addVertex(position + V2f(n, -n), V2f(1.0f, 0.0f), color);
    v.addVertex(position + V2f(n, n), V2f(1.0f, 1.0f), color);
  });
  return v;
}

Mesh buildObstaclesArray(Map& map, GLuint textureID) {
  Mesh v(Color(0.4f, 0.1f, 0.0f));
  v.setTextureID(textureID);
  map.forEachPosition([&](const V2i& p) {
    if (map.tile(p).obstacle) {
      V3f position(v2iToV2f(p), 0.01f);
      float n = 0.5f;
      // first triangle
      v.addVertex(position + V3f(-n, -n) * 0.7f, V2f(0.0f, 0.0f));
      v.addVertex(position + V3f(-n, n) * 0.7f, V2f(0.0f, 1.0f));
      v.addVertex(position + V3f(n, n) * 0.7f, V2f(1.0f, 1.0f));
      // second triangle
      v.addVertex(position + V3f(-n, -n) * 0.7f, V2f(0.0f, 0.0f));
      v.addVertex(position + V3f(n, -n) * 0.7f, V2f(1.0f, 0.0f));
      v.addVertex(position + V3f(n, n) * 0.7f, V2f(1.0f, 1.0f));
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
    float n = 0.5f;
    // first triangle
    v.addVertex(position + V2f(-n, -n), color);
    v.addVertex(position + V2f(-n, n), color);
    v.addVertex(position + V2f(n, n), color);
    // second triangle
    v.addVertex(position + V2f(-n, -n), color);
    v.addVertex(position + V2f(n, -n), color);
    v.addVertex(position + V2f(n, n), color);
  });
  return v;
}
