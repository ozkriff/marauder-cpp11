// See LICENSE file for copyright and license details.

#include "core/lineOfSight.hpp"
#include <cassert>
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include "core/math.hpp"

// http://zvold.blogspot.com/2010/01/bresenhams-line-drawing-algorithm-on_26.html

LineOfSight::LineOfSight(const V2i& from, const V2i& to)
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

bool LineOfSight::isFinished() const {
  return mCurrentPosition == mTo;
}

V2i LineOfSight::getNext() {
  if (isFinished()) {
    return mCurrentPosition;
  }
  if (!mIsSteep) {
    mError += mDelta.y();
    if(mError > abs(mDeltaOld.x())){
      mCurrentPosition = Direction::getNeighbourPosition(
          mCurrentPosition, signsToDirection(mSign.x(), mSign.y()));
      mError -= mDelta.x();
    } else {
      mCurrentPosition = Direction::getNeighbourPosition(
          mCurrentPosition, signsToDirection(0, mSign.x()));
      mError += mDelta.y();
    }
  } else {
    mError += mDelta.x();
    if (mError > 0) {
      mCurrentPosition = Direction::getNeighbourPosition(
          mCurrentPosition, signsToDirection(mSign.x(), mSign.y()));
      mError -= mDelta.y();
    } else {
      mCurrentPosition = Direction::getNeighbourPosition(
          mCurrentPosition, signsToDirection(-mSign.x(), mSign.y()));
      mError += mDelta.y();
    }
  }
  return mCurrentPosition;
}

Direction LineOfSight::signsToDirection(int signX, int signY) {
  struct {
    int signX;
    int signY;
    DirectionID directionID;
  } signsToDirectionMap[] = {
    {-1, -1, DirectionID::NW},
    {0, -1, DirectionID::W},
    {1, -1, DirectionID::NE},
    {-1, 1, DirectionID::SW},
    {0, 1, DirectionID::E},
    {1, 1, DirectionID::SE}
  };
  for (const auto& node : signsToDirectionMap) {
    if (node.signX == signX && node.signY == signY) {
      return Direction(node.directionID);
    }
  }
  throw std::logic_error("bad signs");
}
