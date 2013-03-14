// See LICENSE file for copyright and license details.

#ifndef CORE__MESH_BUILDERS_HPP
#define CORE__MESH_BUILDERS_HPP

#include "visualizer/texture.hpp"

class Mesh;
class Visualizer;
class V2i;
class Color4f;
class Color3u;
class Map;

Mesh buildWalkableMesh(const Map& map);

Color3u fowColor(const Map& map, const V2i& position);

Mesh buildMapMesh(const Map &map, UnsignedInteger textureID);

Mesh buildObstaclesMesh(const Map& map, UnsignedInteger textureID);

Mesh buildUnitCircleMesh(float radius, const Color4f& color);

Mesh buildPickingTilesMesh(const Map& map);

#endif
