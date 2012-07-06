/* See LICENSE file for copyright and license details. */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include "list.h"
#include "misc.h"

void* my_alloc(int count, int size) {
  unsigned int u_count;
  unsigned int u_size;
  assert(count > 0);
  assert(size > 0);
  u_count = (unsigned int)count;
  u_size = (unsigned int)size;
  return calloc(u_count, u_size);
}

void die(const char *message, ...) {
  va_list arg_list;
  va_start(arg_list, message);
  vfprintf(stderr, message, arg_list);
  va_end(arg_list);
  exit(EXIT_FAILURE);
}

void* copy_to_heap(const void *data, int size) {
  void *tmp = calloc(1, size);
  memcpy(tmp, data, (unsigned int)size);
  return tmp;
}

Node* create_node(const void *data, int size) {
  void *newdata = copy_to_heap(data, size);
  Node *n = calloc(1, sizeof(Node));
  set_node(n, newdata);
  return n;
}
