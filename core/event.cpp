// See LICENSE file for copyright and license details.

#include <cassert>
#include <cstdlib>
#include <list>
#include "core/core.h"
#include "core/event.h"
#include "ui_opengl/game.h"

Event::Event() {
}

Event::~Event() {

}

static void undoEvent(Core& core, const Event& e) {
  switch (e.t) {
    case EventTypeID::MOVE: {
      undoEventMove(core, e.e.move);
      break;
    }
    case EventTypeID::END_TURN: {
      // empty
      // TODO: die()?
      break;
    }
    default:
      die("event: undoEvent(): "
          "unknown event type '%d'\n", e.t);
      break;
  }
  // updateUnitsVisibility();
}

// Undo all events that this player have not seen yet
void undoUnshownEvents(Core& core) {
  if (core.events.size() == 0) {
    return;
  }
  auto i = core.events.end();
  --i;
  while (i != core.events.begin()) {
    auto event = *i;
    if (event->id == core.currentPlayer->lastEventID) {
      break;
    }
    undoEvent(core, *event);
    --i;
  }
}

void applyEvent(Core& core, const Event& e) {
  core.currentPlayer->lastEventID = e.id;
  switch (e.t) {
  case EventTypeID::MOVE:
    applyEventMove(core, e.e.move);
    break;
  case EventTypeID::END_TURN:
    applyEventEndTurn(core, e.e.endTurn);
    break;
  default:
    die("event: applyEvent(): "
        "unknown event '%d'\n", e.t);
    break;
  }
  // updateUnitsVisibility();
}

// TODO: rename.
static Event* getNextEventNode(Core& core) {
  // Node *node;
  int id = core.currentPlayer->lastEventID; // shortcut
  if (core.events.size() == 0) {
    return nullptr;
  }
  if (id == HAVE_NOT_SEEN_ANY_EVENTS) {
    return core.events.front();
  }
  // find last seen event
  Event *e = nullptr;
  for (auto e : core.events) {
    if (e->id == id) {
      break;
    }
  }
  if (!e) {
    return nullptr;
  } else {
    return getNext(core.events, e);
  }
}

bool isEventVisible(const Core& core, const Event& e) {
  switch (e.t) {
    case EventTypeID::MOVE: {
      return isVisibleEventMove(core, e.e.move);
    }
    case EventTypeID::END_TURN: {
      return true;
    }
    default: {
      die("event: isEventVisible(): "
          "unknown event '%d'\n", e.t);
      return true;
    }
  }
}

// TODO simplify
void applyInvisibleEvents(Core& core) {
  Event* e = getNextEventNode(core);
  while (e) {
    assert(e);
    if (!isEventVisible(core, *e)) {
      applyEvent(core, *e);
    } else {
      break;
    }
    e = getNext(core.events, e);
    assert(e);
  }
}

// TODO: Called before getNextEvent
bool unshownEventsLeft(Core& core) {
  applyInvisibleEvents(core);
  if (core.events.size() == 0) {
    return false;
  } else {
    auto e = core.events.back();
    return e->id != core.currentPlayer->lastEventID;
  }
}

// Always called after applyInvisibleEvents
Event* getNextEvent(Core& core) {
  int id = core.currentPlayer->lastEventID; // shortcut
  assert(core.events.size() > 0);
  if (id == HAVE_NOT_SEEN_ANY_EVENTS) {
    return core.events.front();
  }
  for (auto e : core.events) {
    if (e->id == id) {
      return getNext(core.events, e);
    }
  }
  return nullptr;
}

static void event2log(const Event& e) {
  UNUSED(e);
  // TODO
}

static void sendEvent(const Event& e) {
  UNUSED(e);
  // TODO
}

void addEvent(Core& core, Event* e) {
  assert(e);
  e->id = core.getNewEventId();
  core.events.push_back(e);
  event2log(*e);
#if 0
  // TODO
  if (!isLocal) {
    sendEvent(e);
  }
#else
  UNUSED(sendEvent);
#endif
}
