/* See LICENSE file for copyright and license details. */

#ifndef CORE_EVENT_H
#define CORE_EVENT_H

enum class EventTypeId {
  E_MOVE,
  E_END_TURN
};

typedef struct {
  EventTypeId t;
  int id;
  union {
    EventMove move;
    EventEndturn end_turn;
  } e;
} Event;

void init_events();
void add_event(Event* e);
Event* get_next_event();
bool unshown_events_left();
void apply_invisible_events();
bool is_event_visible(const Event& e);
void apply_event(const Event& e);
void undo_unshown_events();

#endif
