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

#endif
