// See LICENSE file for copyright and license details.

#include "visualizer/meshBuilders.hpp"
#include "visualizer/visualizer.hpp"

VertexArray buildWalkableArray(Visualizer& visualizer) {
  Core& core = visualizer.core();
  VertexArray v(Color(0.0f, 0.0f, 1.0f), PrimitiveType::Lines);
  core.map().forEachPos([&](const V2i& p) {
    const Tile& t = core.map().tile(p);
    if (t.parent.value() != DirID::NONE && t.cost < 50) {
      V2i to = Dir::getNeighbourPos(p, t.parent);
      if (core.map().isInboard(to)) {
        V2f fromF = visualizer.v2iToV2f(p);
        V2f toF = visualizer.v2iToV2f(to);
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

VertexArray buildMapArray(Visualizer& visualizer, GLuint textureID) {
  Map& map = visualizer.core().map();
  VertexArray v;
  v.setTextureID(textureID);
  map.forEachPos([&](const V2i& p) {
    V2f pos = visualizer.v2iToV2f(p);
    Color3u color = fowColor(map, p);
    for (int i = 0; i < 6; ++i) {
      v.addVertex(
          pos + visualizer.indexToHexVertex(i),
          V2f(0.0f, 0.0f),
          color);
      v.addVertex(
          pos + visualizer.indexToHexVertex(i + 1),
          V2f(1.0f, 0.0f),
          color);
      v.addVertex(
          pos,
          V2f(0.5f, 0.5f),
          color);
    }
  });
  return v;
}

VertexArray buildObstaclesArray(Visualizer& visualizer, GLuint textureID) {
  Map& map = visualizer.core().map();
  VertexArray v(Color(0.4f, 0.1f, 0.0f));
  v.setTextureID(textureID);
  map.forEachPos([&](const V2i& p) {
    if (map.tile(p).obstacle) {
      V2f pos = visualizer.v2iToV2f(p);
      for (int i = 0; i < 6; ++i) {
        v.addVertex(
            V3f(pos + visualizer.indexToHexVertex(i) * 0.7f, 0.01f),
            V2f(0.0f, 0.0f));
        v.addVertex(
            V3f(pos + visualizer.indexToHexVertex(i + 1) * 0.7f, 0.01f),
            V2f(1.0f, 0.0f));
        v.addVertex(
            V3f(pos, 0.01f),
            V2f(0.5f, 0.5f));
      }
    }
  });
  return v;
}

VertexArray buildUnitCircleVertexArray(
    Visualizer& visualizer, float hexIn, const Color& color)
{
  VertexArray v(color, PrimitiveType::Lines);
  const int verticesCount = 12;
  for (int i = 0; i < verticesCount; ++i) {
    const float k = hexIn * 2.0f; // resize coefficient
    const float h = 0.01f;
    v.addVertex(V3f(
        visualizer.indexToCircleVertex(verticesCount, i) * k, h));
    v.addVertex(V3f(
        visualizer.indexToCircleVertex(verticesCount, i + 1) * k, h));
  }
  return v;
}

VertexArray buildPickingTilesArray(Visualizer& visualizer) {
  Map& map = visualizer.core().map();
  VertexArray v;
  map.forEachPos([&](const V2i& p) {
    Color3u color(p.x(), p.y(), 1);
    V2f pos = visualizer.v2iToV2f(p);
    for (int i = 0; i < 6; ++i) {
      v.addVertex(
          V3f(pos + visualizer.indexToHexVertex(i)),
          color);
      v.addVertex(
          V3f(pos + visualizer.indexToHexVertex(i + 1)),
          color);
      v.addVertex(
          V3f(pos),
          color);
    }
  });
  return v;
}
