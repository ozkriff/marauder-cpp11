// See LICENSE file for copyright and license details.

#include "core/los.hpp"
#include <cassert>
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include "core/math.hpp"

Dir Los::getBresDir(int signX, int signY) {
  if(signX == -1 && signY == -1) {
    return(Dir(DirID::NW));
  } else if(signX ==  0 && signY == -1) {
    return(Dir(DirID::W));
  } else if(signX ==  1 && signY == -1) {
    return(Dir(DirID::NE));
  } else if(signX == -1 && signY ==  1) {
    return(Dir(DirID::SW));
  } else if(signX ==  0 && signY ==  1) {
    return(Dir(DirID::E));
  } else if(signX ==  1 && signY ==  1) {
    return(Dir(DirID::SE));
  }
  throw std::logic_error("bad signs");
}

Los::Los(const V2i& from, const V2i& to)
  : mFrom(from),
    mTo(to),
    mCurrentPosition(mFrom),
    mError(0)
{
  mDeltaOld = V2i(
      2 * (to.x() - from.x()) - abs(to.y() % 2) + abs(from.y() % 2),
      to.y() - from.y());
  mDelta = V2i(
    3 * abs(mDeltaOld.x()),
    3 * abs(mDeltaOld.y()));
  mSign = V2i(
    ((mDeltaOld.x() > 0) ? 1 : -1),
    ((mDeltaOld.y() > 0) ? 1 : -1));
  mIsSteep = (abs(mDelta.y()) > abs(mDelta.x()));
}

Los::~Los() {
}

bool Los::isFinished() const {
  return mCurrentPosition == mTo;
}

V2i Los::getNext() {
  if (isFinished()) {
    return mCurrentPosition;
  }
  if (!mIsSteep) {
    mError += mDelta.y();
    if(mError > abs(mDeltaOld.x())){
      mCurrentPosition = Dir::getNeighbourPosition(
          mCurrentPosition, getBresDir(mSign.x(), mSign.y()));
      mError -= mDelta.x();
    } else {
      mCurrentPosition = Dir::getNeighbourPosition(
          mCurrentPosition, getBresDir(0, mSign.x()));
      mError += mDelta.y();
    }
  } else {
    mError += mDelta.x();
    if (mError > 0) {
      mCurrentPosition = Dir::getNeighbourPosition(
          mCurrentPosition, getBresDir(mSign.x(), mSign.y()));
      mError -= mDelta.y();
    } else {
      mCurrentPosition = Dir::getNeighbourPosition(
          mCurrentPosition, getBresDir(-mSign.x(), mSign.y()));
      mError += mDelta.y();
    }
  }
  return mCurrentPosition;
}
