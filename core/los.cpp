/* See LICENSE file for copyright and license details. */

#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include "core/los.h"
#include "core/math.h"

// Wikipedia: Bresenham's line algorithm

Los::Los(const V2i& from, const V2i& to) {
  mFrom = from;
  mTo = to;
  mIsSteep = abs(mTo.y - mFrom.y) > abs(mTo.x - mFrom.x);
  if (mIsSteep) {
    swap_int(&mFrom.x, &mFrom.y);
    swap_int(&mTo.x, &mTo.y);
  }
  if (mFrom.x > mTo.x) {
    swap_int(&mFrom.x, &mTo.x);
    swap_int(&mFrom.y, &mTo.y);
  }
  mDelta.x = mTo.x - mFrom.x;
  mDelta.y = abs(mTo.y - mFrom.y);
  mError = mDelta.x >> 1;
  mPos = mFrom;
  mYstep = (mFrom.y < mTo.y) ? 1 : -1;
}

Los::~Los() {
}

bool Los::isFinished() const {
  return mPos.x >= mTo.x;
}

V2i Los::getNext() {
  V2i n;
  mError -= mDelta.y;
  if (mError < 0) {
    mPos.y += mYstep;
    mError += mDelta.x;
  }
  mPos.x++;
  if (!mIsSteep) {
    n = V2i(mPos.x, mPos.y);
  } else {
    n = V2i(mPos.y, mPos.x);
  }
  return n;
}
