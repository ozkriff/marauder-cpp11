// See LICENSE file for copyright and license details.

#include <cassert>
#include <cstdlib>
#include <list>
#include "core/core.hpp"
#include "core/event.hpp"
#include "ui/game.hpp"

EventTypeID Event::type() const {
  return mType;
}

int Event::id() const {
  return mID;
}

void Event::setID(int id) {
  mID = id;
}

void Event::setType(EventTypeID type) {
  mType = type;
}
