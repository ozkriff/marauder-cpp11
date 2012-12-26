// See LICENSE file for copyright and license details.

#ifndef CORE_LOS_H
#define CORE_LOS_H

#include "core/v2i.h"

class Los {
  V2i mFrom;
  V2i mTo;
  V2i mDelta;
  V2i mPos; // current position
  int mYstep;
  int mError;
  bool mIsSteep;

public:
  Los(const V2i& from, const V2i& to);
  ~Los();

  bool isFinished() const;
  V2i getNext();
};

#endif
