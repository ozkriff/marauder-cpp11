// See LICENSE file for copyright and license details.

#ifndef CORE_EVENT_H
#define CORE_EVENT_H

#include "core/event/move.h"
#include "core/event/end_turn.h"

enum class EventTypeID {
  MOVE,
  END_TURN
};

class Event {
public:
  EventTypeID t;
  int id;
  union TMP {
    EventMove move;
    EventEndturn endTurn;

    TMP() {
      // Due to the Point member, a constructor definition is now required
      new(&move) EventMove();
    }
    ~TMP() {

    }
  } e;

  Event();
  ~Event();
};

void addEvent(Core& core, Event* e);
Event* getNextEvent(Core& core);
bool unshownEventsLeft(Core& core);
void applyInvisibleEvents(Core& core);
bool isEventVisible(const Core& core, const Event& e);
void applyEvent(Core& core, const Event& e);
void undoUnshownEvents(Core& core);

#endif
