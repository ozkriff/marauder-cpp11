// See LICENSE file for copyright and license details.

#ifndef CORE__LOS_HPP
#define CORE__LOS_HPP

#include "core/v2i.hpp"
#include "core/dir.hpp"

// http://zvold.blogspot.com/2010/01/bresenhams-line-drawing-algorithm-on_26.html

class Los {
public:
  Los(const V2i& from, const V2i& to);
  ~Los();

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
