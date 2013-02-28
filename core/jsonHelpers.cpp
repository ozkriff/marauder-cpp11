// See LICENSE file for copyright and license details.

#include "core/jsonHelpers.hpp"
#include <fstream>
#include <stdexcept>
#include "json/json.h"
#include "core/v2i.hpp"

Json::Value parseConfig(const std::string& fileName) {
  Json::Value node;
  Json::Reader reader;
  std::ifstream f(fileName);
  bool isOk = reader.parse(f, node);
  if (!isOk) {
    throw std::runtime_error(
        std::string("can't parse json config:")
            + reader.getFormatedErrorMessages());
  }
  return node;
}

Json::Value parseConfigFromString(const std::string& config) {
  Json::Value node;
  Json::Reader reader;
  bool isOk = reader.parse(config, node);
  if (!isOk) {
    throw std::runtime_error(
        std::string("can't parse json string:")
            + reader.getFormatedErrorMessages());
  }
  return node;
}

// TODO: make templated func with specializations
V2i JsonValueToV2i(const Json::Value& value) {
  return V2i(value["x"].asInt(), value["y"].asInt());
}
