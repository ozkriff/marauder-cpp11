// See LICENSE file for copyright and license details.

#ifndef CORE__UNIT_HPP
#define CORE__UNIT_HPP

#include "core/dir.hpp"
#include "core/v2i.hpp"

class UnitType;

class Unit {
public:
  Unit(int id, int playerID, const UnitType& type);
  ~Unit();

  const UnitType& type() const;
  int id() const;
  int playerID() const;
  const Dir& direction() const;
  int actionPoints() const;
  const V2i& position() const;

  // TODO: remove some of this setters :(
  void setDirection(const Dir& direction);
  void setActionPoints(int actionPoints);
  void setPosition(const V2i& position);

private:
  const UnitType& mType;
  int mID;
  int mPlayerID;
  Dir mDirection;
  int mActionPoints;
  V2i mPosition;
};

#endif
