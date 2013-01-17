// See LICENSE file for copyright and license details.

#ifndef CORE__MISC_HPP
#define CORE__MISC_HPP

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

#define UNUSED(x) ((void)x)

#endif
