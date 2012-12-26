// See LICENSE file for copyright and license details.

#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include "core/los.h"
#include "core/math.h"

// Wikipedia: Bresenham's line algorithm

Los::Los(const V2i& from, const V2i& to) {
  mFrom = from;
  mTo = to;
  mIsSteep = abs(mTo.y() - mFrom.y()) > abs(mTo.x() - mFrom.x());
  if (mIsSteep) {
    mFrom = V2i(mFrom.y(), mFrom.x());
    mTo = V2i(mTo.y(), mTo.x());
  }
  if (mFrom.x() > mTo.x()) {
    std::swap(mFrom, mTo);
    // V2i tmp = mFrom;
    // mFrom = mTo;
    // mTo = tmp;
  }
  mDelta.setX(mTo.x() - mFrom.x());
  mDelta.setY(abs(mTo.y() - mFrom.y()));
  mError = mDelta.x() >> 1;
  mPos = mFrom;
  mYstep = (mFrom.y() < mTo.y()) ? 1 : -1;
}

Los::~Los() {
}

bool Los::isFinished() const {
  return mPos.x() >= mTo.x();
}

V2i Los::getNext() {
  mError -= mDelta.y();
  if (mError < 0) {
    mPos.setY(mPos.y() + mYstep);
    mError += mDelta.x();
  }
  mPos.setX(mPos.x() + 1);
  if (!mIsSteep) {
    return mPos;
  } else {
    return V2i(mPos.y(), mPos.x());
  }
}
