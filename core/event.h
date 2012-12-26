// See LICENSE file for copyright and license details.

#ifndef CORE_EVENT_H
#define CORE_EVENT_H

#include "core/event/move.h"
#include "core/event/end_turn.h"

enum class EventTypeId {
  E_MOVE,
  E_END_TURN
};

class Event {
public:
  EventTypeId t;
  int id;
  union TMP {
    EventMove move;
    EventEndturn end_turn;

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

void init_events();
void add_event(Core& core, Event* e);
Event* get_next_event(Core& core);
bool unshown_events_left(Core& core);
void apply_invisible_events(Core& core);
bool is_event_visible(const Core& core, const Event& e);
void apply_event(Core& core, const Event& e);
void undo_unshown_events(Core& core);

#endif
