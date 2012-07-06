/* See LICENSE file for copyright and license details. */

void* my_alloc(int count, int size);
#define ALLOCATE(count, type) \
  my_alloc(count, sizeof(type))

void die(const char *message, ...);

#define COPY_TO_HEAP(data, type) \
  create_node(data, sizeof(type))

Node* create_node(const void *data, int size);
void* copy_to_heap(const void *data, int size);

#define UNUSED(x) ((void)x)
