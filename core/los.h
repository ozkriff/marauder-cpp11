// See LICENSE file for copyright and license details.

#ifndef CORE_LOS_H
#define CORE_LOS_H

#include "core/v2i.h"
#include "core/dir.h"

// http://zvold.blogspot.com/2010/01/bresenhams-line-drawing-algorithm-on_26.html

class Los {
  V2i mFrom;
  V2i mTo;
  V2i mDelta;
  V2i mCurrentPos;
  int mError;
  bool mIsSteep;
  V2i mSign;
  V2i mDeltaOld;
  
  static Dir getBresDir(int signX, int signY);

public:
  Los(const V2i& from, const V2i& to);
  ~Los();

  bool isFinished() const;
  V2i getNext();
};

#endif
