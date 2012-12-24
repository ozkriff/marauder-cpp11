/* See LICENSE file for copyright and license details. */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include "core/misc.h"

void die(const char *message, ...) {
  va_list arg_list;
  va_start(arg_list, message);
  vfprintf(stderr, message, arg_list);
  va_end(arg_list);
  exit(EXIT_FAILURE);
}

#if 0
Node* create_node(const void *data, int size) {
  void *newdata = copy_to_heap(data, size);
  Node *n = new Node;
  set_node(n, newdata);
  return n;
}
#endif
