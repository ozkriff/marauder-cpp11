/* See LICENSE file for copyright and license details. */

#include <list>

template <class T>
T* getNext(std::list<T*> &list, T* node) {
  auto it = list.begin();
  while (it != list.end()) {
    if (*it == node) {
      ++it;
      return *it;
    }
    ++it;
  }
  return nullptr;
}

void die(const char *message, ...);

#define COPY_TO_HEAP(data, type) \
  create_node(data, sizeof(type))

// Node* create_node(const void *data, int size);
void* copy_to_heap(const void *data, int size);

#define UNUSED(x) ((void)x)
#define CAST(data, type) ((type)(data))
