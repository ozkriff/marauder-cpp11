// See LICENSE file for copyright and license details.

#ifndef CORE__UNIT_TYPE_HPP
#define CORE__UNIT_TYPE_HPP

enum class UnitTypeID {
  TANK,
  TRUCK,
  COUNT
};

struct UnitType {
  int rangeOfVision;
  int actionPoints;
};

const UnitType& getUnitType(int id);
void initUnitTypes();

#endif
