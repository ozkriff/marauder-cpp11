// See LICENSE file for copyright and license details.

#ifndef CORE_PATHQUEUE_H
#define CORE_PATHQUEUE_H

#include <vector>
#include "core/dir.hpp"

class Core;

class PathQueue {
private:
  Core& mCore;
  unsigned int mTailNodeIndex;
  unsigned int mHeadNodeIndex;
  std::vector<V2i> mNodes;

public:
  PathQueue(Core& core, int size);
  ~PathQueue();

  bool isEmpty() const;
  void push(const V2i& m, Dir parent, int newcost, Dir dir);
  V2i pop();
};

#endif
