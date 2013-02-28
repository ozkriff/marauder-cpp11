// See LICENSE file for copyright and license details.

#ifndef CORE__JSON_HELPERS_HPP
#define CORE__JSON_HELPERS_HPP

#include <string>
#include "json/forwards.h"

class V2i;

Json::Value parseConfig(const std::string& fileName);
Json::Value parseConfigFromString(const std::string& config);
V2i JsonValueToV2i(const Json::Value& value);

#endif
