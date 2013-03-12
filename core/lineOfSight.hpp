// See LICENSE file for copyright and license details.

#ifndef CORE__LINE_OF_SIGHT_HPP
#define CORE__LINE_OF_SIGHT_HPP

#include "core/v2i.hpp"
#include "core/dir.hpp"

class LineOfSight {
public:
  LineOfSight(const V2i& from, const V2i& to);
  ~LineOfSight();

  bool isFinished() const;
  V2i getNext();

private:
  V2i mFrom;
  V2i mTo;
  V2i mDelta;
  V2i mCurrentPosition;
  int mStepY;
  int mError;
  bool mIsSteep;
};

#endif
