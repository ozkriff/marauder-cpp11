// See LICENSE file for copyright and license details.

#ifndef CORE__JSON_HELPERS_HPP
#define CORE__JSON_HELPERS_HPP

#include <string>
#include "json/forwards.h"
#include "core/v2i.hpp"

Json::Value parseConfig(const std::string& fileName);
V2i JsonValueToV2i(const Json::Value& value);

#endif
