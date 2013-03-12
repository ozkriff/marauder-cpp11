// See LICENSE file for copyright and license details.

#include "core/unit.hpp"
#include <cassert>

Unit::Unit(UnitID id, PlayerID playerID, const UnitType &type)
  : mType(type),
    mID(id),
    mPlayerID(playerID),
    mDirection(DirectionID::NONE),
    mActionPoints(0),
    mPosition(0, 0)
{
}

Unit::~Unit() {
}

const UnitType& Unit::type() const {
  return mType;
}

UnitID Unit::id() const {
  return mID;
}

PlayerID Unit::playerID() const {
  return mPlayerID;
}

const Direction& Unit::direction() const {
  return mDirection;
}

int Unit::actionPoints() const {
  return mActionPoints;
}

const V2i& Unit::position() const {
  return mPosition;
}

void Unit::setDirection(const Direction& direction) {
  mDirection = direction;
}

void Unit::setActionPoints(int actionPoints) {
  mActionPoints = actionPoints;
}

void Unit::setPosition(const V2i& position) {
  mPosition = position;
}
