// See LICENSE file for copyright and license details.

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cassert>
#include <cstring>
#include <fstream>
#include "json/json.h"
#include "core/misc.hpp"

Json::Value parseConfig(const std::string& fileName) {
  Json::Value node;
  Json::Reader reader;
  std::ifstream f(fileName);
  reader.parse(f, node);
  return node;
}

void die(const char *message, ...) {
  va_list argList;
  va_start(argList, message);
  vfprintf(stderr, message, argList);
  va_end(argList);
  exit(EXIT_FAILURE);
}
