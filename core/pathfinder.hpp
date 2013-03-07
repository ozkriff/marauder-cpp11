// See LICENSE file for copyright and license details.

#ifndef CORE__PATHFINDER_HPP
#define CORE__PATHFINDER_HPP

#include <vector>
#include <queue>
#include "core/dir.hpp"

class Unit;
class Core;

class Pathfinder {
public:
  Pathfinder(Core& core);

  std::vector<V2i> getPath(const V2i& position) const;
  void fillMap(const Unit& u);
  void cleanMap();

private:
  std::queue<V2i> mQueue;
  Core& mCore;

  void tryToPushNeighbours(const Unit& u, const V2i& m);

  // TODO: rename
  // p1 - origPosition, p2 - neighbour position
  void processNeighbourPosition(const Unit& u, const V2i& p1, const V2i& p2);

  Dir getParentDir(const Unit& u, const V2i& m);
  int getTileCost(const Unit& u, const V2i& t, const V2i& nb);
};

#endif
