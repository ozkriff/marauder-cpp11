// See LICENSE file for copyright and license details.

#include "core/unit.hpp"
#include <cassert>

Unit::Unit(int id, PlayerID playerID, const UnitType &type)
  : mType(type),
    mID(id),
    mPlayerID(playerID),
    mDirection(DirID::NONE),
    mActionPoints(0),
    mPosition(0, 0)
{
}

Unit::~Unit() {
}

const UnitType& Unit::type() const {
  return mType;
}

int Unit::id() const {
  return mID;
}

PlayerID Unit::playerID() const {
  return mPlayerID;
}

const Dir& Unit::direction() const {
  return mDirection;
}

int Unit::actionPoints() const {
  return mActionPoints;
}

const V2i& Unit::position() const {
  return mPosition;
}

void Unit::setDirection(const Dir& direction) {
  mDirection = direction;
}

void Unit::setActionPoints(int actionPoints) {
  mActionPoints = actionPoints;
}

void Unit::setPosition(const V2i& position) {
  mPosition = position;
}
