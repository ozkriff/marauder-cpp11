// See LICENSE file for copyright and license details.

#ifndef CORE__MESH_BUILDERS_HPP
#define CORE__MESH_BUILDERS_HPP

#include "SDL_opengl.h"

class Mesh;
class Visualizer;
class V2i;
class Color;
class Color3u;
class Map;

Mesh buildWalkableArray(Visualizer& visualizer);

Color3u fowColor(const Map& map, const V2i& position);

Mesh buildMapArray(Visualizer& visualizer, GLuint textureID);

Mesh buildObstaclesArray(Visualizer& visualizer, GLuint textureID);

Mesh buildUnitCircleMesh(
    Visualizer& visualizer, float radius, const Color& color);

Mesh buildPickingTilesArray(Visualizer& visualizer);

#endif
