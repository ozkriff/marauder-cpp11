// See LICENSE file for copyright and license details.

#include <cassert>
#include <cstdlib>
#include <list>
#include "core/core.hpp"
#include "core/event.hpp"

Event::Event(int id, EventTypeID type)
  : mType(type),
    mID(id)
{
}

Event::~Event() {
}

EventTypeID Event::type() const {
  return mType;
}

int Event::id() const {
  return mID;
}
