// See LICENSE file for copyright and license details.

#ifndef CORE__MESH_BUILDERS_HPP
#define CORE__MESH_BUILDERS_HPP

#include "SDL_opengl.h"

class VertexArray;
class Visualizer;
class V2i;
class Color;
class Color3u;
class Map;

VertexArray buildWalkableArray(Visualizer& visualizer);

Color3u fowColor(const Map& map, const V2i& position);

VertexArray buildMapArray(Visualizer& visualizer, GLuint textureID);

VertexArray buildObstaclesArray(Visualizer& visualizer, GLuint textureID);

VertexArray buildUnitCircleVertexArray(
    Visualizer& visualizer, float hexIn, const Color& color);

VertexArray buildPickingTilesArray(Visualizer& visualizer);

#endif
