// See LICENSE file for copyright and license details.

#include <cassert>
#include "core/core.hpp"

// public

EventManager::EventManager(Core& core)
  : mCore(core),
    mCurrentEvent(nullptr)
{
}

EventManager::~EventManager() {
}

Event& EventManager::currentEvent() {
  return *mCurrentEvent;
}

void EventManager::setCurrentEvent(Event* event) {
  mCurrentEvent = event;
}

void EventManager::addEvent(Event* e) {
  assert(e);
  mEvents.push_back(e);
#if 0 // TODO
  // event2log(*e);
  if (!isLocal) {
    sendEvent(e);
  }
#endif
}

// Always called after applyInvisibleEvents
Event* EventManager::getNextEvent() {
  int id = mCore.currentPlayer().lastSeenEventID; // shortcut
  assert(!mEvents.empty());
  if (id == HAVE_NOT_SEEN_ANY_EVENTS) {
    return mEvents.front();
  }
  for (auto e : mEvents) {
    if (e->id() == id) {
      return getNext(mEvents, e);
    }
  }
  return nullptr;
}

// TODO: Called before getNextEvent
bool EventManager::unshownEventsLeft() {
  applyInvisibleEvents();
  if (mEvents.empty()) {
    return false;
  } else {
    auto e = mEvents.back();
    return e->id() != mCore.currentPlayer().lastSeenEventID;
  }
}

// TODO simplify
void EventManager::applyInvisibleEvents() {
  Event* e = getNextEventNode();
  while (e) {
    assert(e);
    if (!isEventVisible(*e)) {
      applyEvent(*e);
    } else {
      break;
    }
    e = getNext(mEvents, e);
    assert(e);
  }
}

bool EventManager::isEventVisible(const Event& e) const {
  return e.isVisible(mCore);
}

void EventManager::applyEvent(Event& e) {
  mCore.currentPlayer().lastSeenEventID = e.id();
  e.apply(mCore);
}

// Undo all events that this player have not seen yet
void EventManager::undoUnshownEvents() {
  if (mEvents.empty()) {
    return;
  }
  auto i = mEvents.end();
  --i;
  while (i != mEvents.begin()) {
    auto event = *i;
    if (event->id() == mCore.currentPlayer().lastSeenEventID) {
      break;
    }
    undoEvent(*event);
    --i;
  }
}

int EventManager::getNewEventID() {
  if (!mEvents.empty()) {
    return mEvents.back()->id() + 1;
  } else {
    return 0;
  }
}

// private

void EventManager::undoEvent(Event& e) {
  e.undo(mCore);
}

// TODO: rename.
Event* EventManager::getNextEventNode() {
  int id = mCore.currentPlayer().lastSeenEventID; // shortcut
  if (mEvents.empty()) {
    return nullptr;
  }
  if (id == HAVE_NOT_SEEN_ANY_EVENTS) {
    return mEvents.front();
  }
  // find last seen event
  for (auto e : mEvents) {
    if (e->id() == id) {
      return getNext(mEvents, e);
    }
  }
  return nullptr; // if there is no event with that id
}

void EventManager::event2log(const Event& e) {
  UNUSED(e);
  // TODO
}

void EventManager::sendEvent(const Event& e) {
  UNUSED(e);
  // TODO
}
