/* See LICENSE file for copyright and license details. */

#ifndef CORE_PATH_H
#define CORE_PATH_H

#include <vector>
#include "core/dir.h"

class Unit;
class Core;

class PathQueue {
private:
  Core& core;
  unsigned int mTailNodeIndex;
  unsigned int mHeadNodeIndex;
  std::vector<V2i> mNodes;

public:
  PathQueue(Core& core, int size);
  ~PathQueue();

  bool is_empty() const;
  void push(const V2i& m, Dir parent, int newcost, Dir dir);
  V2i pop();
};

class Pathfinder {
private:
  PathQueue q;
  Core& core;

  void try_to_push_neibors(const Unit& u, const V2i& m);
  void process_neibor(const Unit& u, const V2i& p1, const V2i& p2);
  bool can_move_there(const V2i& p1, const V2i& p2);
  Dir get_parent_dir(const Unit& u, const V2i& m);
  int get_tile_cost(const Unit& u, const V2i& t, const V2i& nb);

public:
  Pathfinder(Core& core);

  std::vector<V2i> get_path(const V2i& pos);
  void fill_map(const Unit& u);
  void clean_map();
};

#endif
