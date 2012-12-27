// See LICENSE file for copyright and license details.

#ifndef CORE_UNIT_TYPE_H
#define CORE_UNIT_TYPE_H

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
