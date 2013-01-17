// See LICENSE file for copyright and license details.

#include <cstdlib>
#include <cassert>
#include "core/pathfinder.hpp"
#include "core/core.hpp"

PathQueue::PathQueue(Core& core, int size)
  : mCore(core),
    mTailNodeIndex(0),
    mHeadNodeIndex(0),
    mNodes(size)
{
  assert(size > 0);
}

PathQueue::~PathQueue() {
}

bool PathQueue::isEmpty() const {
  return (mHeadNodeIndex == mTailNodeIndex);
}

void PathQueue::push(const V2i& m, Dir parent, int newcost, Dir dir) {
  Tile& t = mCore.tile(m);
  t.cost = newcost;
  t.parent = parent;
  t.dir = dir;
  mNodes[mTailNodeIndex] = m;
  mTailNodeIndex++;
  if (mTailNodeIndex == mNodes.size()) {
    mTailNodeIndex = 0;
  }
}

V2i PathQueue::pop() {
  assert(mHeadNodeIndex != mTailNodeIndex);
  V2i m = mNodes[mHeadNodeIndex];
  mHeadNodeIndex++;
  if (mHeadNodeIndex == mNodes.size()) {
    mHeadNodeIndex = 0;
  }
  return m;
}
