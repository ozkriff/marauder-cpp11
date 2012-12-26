// See LICENSE file for copyright and license details.

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cassert>
#include <cstring>
#include "core/misc.h"

void die(const char *message, ...) {
  va_list arg_list;
  va_start(arg_list, message);
  vfprintf(stderr, message, arg_list);
  va_end(arg_list);
  exit(EXIT_FAILURE);
}
