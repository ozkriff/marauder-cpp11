// See LICENSE file for copyright and license details.

#ifndef CORE__LINE_OF_SIGHT_HPP
#define CORE__LINE_OF_SIGHT_HPP

#include "core/v2i.hpp"
#include "core/direction.hpp"

class LineOfSight {
public:
  LineOfSight(const V2i& from, const V2i& to);

  bool isFinished() const;
  V2i getNext();

private:
  V2i mFrom;
  V2i mTo;
  V2i mDelta;
  V2i mCurrentPosition;
  int mError;
  bool mIsSteep;
  V2i mSign;
  V2i mDeltaOld;

  static Direction signsToDirection(int signX, int signY);
};

#endif
