// See LICENSE file for copyright and license details.

#include <cassert>
#include "core/unitType.hpp"

namespace {

UnitType unitTypes[static_cast<int>(UnitTypeID::COUNT)];

void initTank(UnitType* t) {
  assert(t);
  t->rangeOfVision = 4;
  t->actionPoints = 4;
}

void initTruck(UnitType* t) {
  assert(t);
  t->rangeOfVision = 5;
  t->actionPoints = 6;
}

} // namespace

const UnitType& getUnitType(int id) {
  assert(id >= 0 && id < static_cast<int>(UnitTypeID::COUNT));
  return unitTypes[id];
}

void initUnitTypes() {
  initTank(unitTypes + static_cast<int>(UnitTypeID::TANK));
  initTruck(unitTypes + static_cast<int>(UnitTypeID::TRUCK));
}
