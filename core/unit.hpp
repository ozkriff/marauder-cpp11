// See LICENSE file for copyright and license details.

#ifndef CORE__UNIT_HPP
#define CORE__UNIT_HPP

#include "core/direction.hpp"
#include "core/v2i.hpp"
#include "core/player.hpp"

class UnitType;

typedef int UnitID;

class Unit {
public:
  Unit(UnitID id, PlayerID playerID, const UnitType& type);
  ~Unit();

  const UnitType& type() const;
  UnitID id() const;
  PlayerID playerID() const;
  const Direction& direction() const;
  int actionPoints() const;
  const V2i& position() const;

  // TODO: remove some of this setters :(
  void setDirection(const Direction& direction);
  void setActionPoints(int actionPoints);
  void setPosition(const V2i& position);

private:
  const UnitType& mType;
  UnitID mID;
  PlayerID mPlayerID;
  Direction mDirection;
  int mActionPoints;
  V2i mPosition;
};

#endif
