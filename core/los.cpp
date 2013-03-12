// See LICENSE file for copyright and license details.

#include "core/los.hpp"
#include <cassert>
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include "core/math.hpp"

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
  }
  mDelta.setX(mTo.x() - mFrom.x());
  mDelta.setY(abs(mTo.y() - mFrom.y()));
  mError = mDelta.x() >> 1;
  mCurrentPosition = mFrom;
  mStepY = (mFrom.y() < mTo.y()) ? 1 : -1;
}

Los::~Los() {
}

bool Los::isFinished() const {
  return mCurrentPosition.x() >= mTo.x();
}

V2i Los::getNext() {
  mError -= mDelta.y();
  if (mError < 0) {
    mCurrentPosition.setY(mCurrentPosition.y() + mStepY);
    mError += mDelta.x();
  }
  if (isFinished()) {
    return mCurrentPosition;
  }
  mCurrentPosition.setX(mCurrentPosition.x() + 1);
  if (!mIsSteep) {
    return mCurrentPosition;
  } else {
    return V2i(mCurrentPosition.y(), mCurrentPosition.x());
  }
}
