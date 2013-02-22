// See LICENSE file for copyright and license details.

#include <cassert>
#include <string>
#include <map>
#include "json/json.h"
#include "core/unitType.hpp"
#include "core/jsonHelpers.hpp"

namespace {

static std::map<std::string, UnitType> unitTypes;

UnitType parseUnitTypeInfo(const Json::Value& unitTypeInfo) {
  UnitType unitType;
  unitType.rangeOfVision = unitTypeInfo["rangeOfVision"].asInt();
  unitType.actionPoints = unitTypeInfo["actionPoints"].asInt();
  unitType.id = unitTypes.size();
  return unitType;
}

} // namespace

const UnitType& getUnitType(const std::string& name) {
  assert(unitTypes.count(name) != 0);
  return unitTypes[name];
}

void initUnitTypes() {
  Json::Value config = parseConfig("unitTypes.json");
  for (const std::string& unitTypeName : config.getMemberNames()) {
    UnitType unitType = parseUnitTypeInfo(config[unitTypeName]);
    unitTypes[unitTypeName] = unitType;
  }
}
