// See LICENSE file for copyright and license details.

#ifndef CORE_PATH_H
#define CORE_PATH_H

#include <vector>
#include "core/dir.hpp"
#include "core/pathQueue.hpp"

class Unit;
class Core;

class Pathfinder {
private:
  PathQueue mQueue;
  Core& mCore;

  void tryToPushNeibors(const Unit& u, const V2i& m);
  void processNeibor(const Unit& u, const V2i& p1, const V2i& p2);
  bool canMoveThere(const V2i& p1, const V2i& p2);
  Dir getParentDir(const Unit& u, const V2i& m);
  int getTileCost(const Unit& u, const V2i& t, const V2i& nb);

public:
  Pathfinder(Core& core);

  std::vector<V2i> getPath(const V2i& pos);
  void fillMap(const Unit& u);
  void cleanMap();
};

#endif
