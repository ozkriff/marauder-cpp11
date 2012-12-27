// See LICENSE file for copyright and license details.

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cassert>
#include <cstring>
#include "core/misc.h"

void die(const char *message, ...) {
  va_list argList;
  va_start(argList, message);
  vfprintf(stderr, message, argList);
  va_end(argList);
  exit(EXIT_FAILURE);
}
