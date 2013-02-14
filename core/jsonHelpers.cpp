// See LICENSE file for copyright and license details.

#include "core/jsonHelpers.hpp"

V2i JsonValueToV2i(const Json::Value& value) {
  return V2i(value["x"].asInt(), value["y"].asInt());
}
