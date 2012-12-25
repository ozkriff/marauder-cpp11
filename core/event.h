/* See LICENSE file for copyright and license details. */

#ifndef CORE_EVENT_H
#define CORE_EVENT_H

typedef enum {
  E_MOVE,
  E_END_TURN
} EventTypeId;

typedef struct {
  EventTypeId t;
  int id;
  union {
    EventMove move;
    EventEndturn end_turn;
  } e;
} Event;

void init_events(void);
void add_event(Event *e);
Event* get_next_event(void);
bool unshown_events_left(void);
void apply_invisible_events(void);
bool is_event_visible(const Event *e);
void apply_event(const Event *e);
void undo_unshown_events(void);

#endif
