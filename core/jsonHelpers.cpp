// See LICENSE file for copyright and license details.

#include "core/jsonHelpers.hpp"
#include <fstream>
#include "json/json.h"
#include "core/v2i.hpp"

Json::Value parseConfig(const std::string& fileName) {
  Json::Value node;
  Json::Reader reader;
  std::ifstream f(fileName);
  reader.parse(f, node);
  return node;
}

V2i JsonValueToV2i(const Json::Value& value) {
  return V2i(value["x"].asInt(), value["y"].asInt());
}
