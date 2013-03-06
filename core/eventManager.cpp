// See LICENSE file for copyright and license details.

#include <cassert>
#include <stdexcept>
#include "core/core.hpp"
#include "core/player.hpp"
#include "core/event.hpp"

// public

EventManager::EventManager(Core& core)
{
  UNUSED(core);
}

EventManager::~EventManager() {
}

void EventManager::addEvent(Event* e) {
  assert(e);
  mNewEvents.push_back(e);
}

bool EventManager::isEmpty() {
  return mNewEvents.empty();
}

Event* EventManager::popEvent() {
  if (isEmpty()) {
    throw std::logic_error("no new events");
  }
  Event* tmp = mNewEvents.front();
  mNewEvents.pop_front();
  return tmp;
}
