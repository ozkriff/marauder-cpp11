// See LICENSE file for copyright and license details.

#ifndef CORE__UNIT_TYPE_HPP
#define CORE__UNIT_TYPE_HPP

// TODO: private fields? getters?
class UnitType {
public:
  int id;
  int rangeOfVision;
  int actionPoints;
};

const UnitType& getUnitType(const std::string& name);
void initUnitTypes();

#endif
