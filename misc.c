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
  void *mem;
  assert(count > 0);
  assert(size > 0);
  u_count = CAST(count, unsigned int);
  u_size = CAST(size, unsigned int);
  mem = calloc(u_count, u_size);
  /* For debugging */
  memset(mem, 0xCC, u_count * u_size);
  return mem;
}

void my_free(void **mem) {
  assert(mem);
  assert(*mem);
  free(*mem);
  *mem = NULL;
}

void die(const char *message, ...) {
  va_list arg_list;
  va_start(arg_list, message);
  vfprintf(stderr, message, arg_list);
  va_end(arg_list);
  exit(EXIT_FAILURE);
}

void* copy_to_heap(const void *data, int size) {
  void *tmp;
  assert(data);
  assert(size > 0);
  tmp = calloc(1, size);
  memcpy(tmp, data, CAST(size, unsigned int));
  return tmp;
}

Node* create_node(const void *data, int size) {
  void *newdata = copy_to_heap(data, size);
  Node *n = ALLOCATE(1, Node);
  set_node(n, newdata);
  return n;
}
