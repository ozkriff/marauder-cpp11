// See LICENSE file for copyright and license details.

#ifndef CORE__PATHQUEUE_HPP
#define CORE__PATHQUEUE_HPP

#include <vector>
#include "core/dir.hpp"

class Core;

class PathQueue {
public:
  PathQueue(Core& core, int size);
  ~PathQueue();

  bool isEmpty() const;
  void push(const V2i& m, Dir parent, int newcost, Dir dir);
  V2i pop();

private:
  Core& mCore;
  unsigned int mTailNodeIndex;
  unsigned int mHeadNodeIndex;
  std::vector<V2i> mNodes;
};

#endif
