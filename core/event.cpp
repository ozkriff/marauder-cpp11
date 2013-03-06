// See LICENSE file for copyright and license details.

#include <cassert>
#include <cstdlib>
#include <list>
#include "core/core.hpp"
#include "core/event.hpp"

Event::Event(EventType type)
  : mType(type)
{
}

Event::~Event() {
}

EventType Event::type() const {
  return mType;
}
