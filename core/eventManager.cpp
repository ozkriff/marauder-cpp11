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
  for (auto e : mEvents) {
    delete e;
  }
}

const Event& EventManager::currentEvent() const {
  return *mCurrentEvent;
}

void EventManager::switchToNextEvent() {
  mCurrentEvent = getNextEvent();
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

bool EventManager::unshownEventsLeft() {
  applyInvisibleEvents();
  if (mEvents.empty()) {
    return false;
  } else {
    auto e = mEvents.back();
    return e->id() != mCore.currentPlayer().lastSeenEventID;
  }
}

void EventManager::applyCurrentEvent() {
  applyEvent(currentEvent());
  mCurrentEvent = nullptr;
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

int EventManager::getNewEventID() const {
  if (!mEvents.empty()) {
    return mEvents.back()->id() + 1;
  } else {
    return 0;
  }
}

// private

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

void EventManager::applyEvent(const Event& e) {
  mCore.currentPlayer().lastSeenEventID = e.id();
  e.apply(mCore);
}

void EventManager::applyInvisibleEvents() {
  for (const Event* e = getNextEventNode();
      e && !isEventVisible(*e);
      e = getNext(mEvents, e))
  {
    assert(e);
    applyEvent(*e);
  }
}

bool EventManager::isEventVisible(const Event& e) const {
  return e.isVisible(mCore);
}

void EventManager::undoEvent(const Event& e) {
  e.undo(mCore);
}

// TODO: rename.
const Event* EventManager::getNextEventNode() {
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
