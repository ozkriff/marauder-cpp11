// See LICENSE file for copyright and license details.

#ifndef CORE__MISC_HPP
#define CORE__MISC_HPP

#include <cassert>
#include <list>
#include <string>

namespace Json {
  class Value;
}

Json::Value parseConfig(const std::string& fileName);

// TODO: Rewrite :(
template <class T>
T* getNext(const std::list<T*> &list, const T* node) {
  assert(node);
  for (auto it = list.begin(); it != list.end(); ++it) {
    if (*it == node) {
      ++it;
      if (it != list.end()) {
        return *it;
      }
      return nullptr;
    }
  }
  return nullptr;
}

void die(const char *message, ...);

#define UNUSED(x) ((void)x)

#endif
