// See LICENSE file for copyright and license details.

#include <cassert>
#include "core/unitType.hpp"

static UnitType unitTypes[static_cast<int>(UnitTypeID::COUNT)];

static void initTank(UnitType* t) {
  assert(t);
  t->rangeOfVision = 4;
  t->actionPoints = 5;
}

static void initTruck(UnitType* t) {
  assert(t);
  t->rangeOfVision = 6;
  t->actionPoints = 8;
}

const UnitType& getUnitType(int id) {
  assert(id >= 0 && id < static_cast<int>(UnitTypeID::COUNT));
  return unitTypes[id];
}

void initUnitTypes() {
  initTank(unitTypes + static_cast<int>(UnitTypeID::TANK));
  initTruck(unitTypes + static_cast<int>(UnitTypeID::TRUCK));
}
