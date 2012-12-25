/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include <stdlib.h>
#include <list>
#include "core/v2i.h"
#include "core/dir.h"
#include "core/misc.h"
#include "core/unit_type.h"
#include "core/core.h"
#include "core/event/move.h"
#include "core/event/end_turn.h"
#include "core/event.h"
#include "core/path.h"
#include "core/los.h"
#include "core/core_private.h"

static std::list<Event*> events;

void init_events(void) {
}

static void undo_event(const Event *e) {
  assert(e);
  switch (e->t) {
    case E_MOVE: {
      undo_event_move(&e->e.move);
      break;
    }
    case E_END_TURN: {
      /* empty */
      /* TODO: die()? */
      break;
    }
    default:
      die("event: undo_event(): "
          "unknown event type '%d'\n", e->t);
      break;
  }
  /* update_units_visibility(); */
}

/* Undo all events that this player have not seen yet */
void undo_unshown_events(void) {
  if (events.size() == 0) {
    return;
  }
  std::list<Event*>::iterator i = events.end();
  --i;
  while (i != events.begin()) {
    auto event = *i;
    if (event->id == current_player->last_event_id) {
      break;
    }
    undo_event(event);
    --i;
  }
}

void apply_event(const Event *e) {
  assert(e);
  current_player->last_event_id = e->id;
  switch (e->t) {
    case E_MOVE: {
      apply_event_move(&e->e.move);
      break;
    }
    case E_END_TURN: {
      apply_event_end_turn(&e->e.end_turn);
      break;
    }
    default: {
      die("event: apply_event(): "
          "unknown event '%d'\n", e->t);
      break;
    }
  }
  /* update_units_visibility(); */
}

/* TODO: rename. */
static Event* get_next_event_node(void) {
  // Node *node;
  int id = current_player->last_event_id; /* shortcut */
  if (events.size() == 0) {
    return nullptr;
  }
  if (id == HAVE_NOT_SEEN_ANY_EVENTS) {
    return events.front();
  }
  /* find last seen event */
  Event *e = nullptr;
  for (auto e : events) {
    if (e->id == id)
      break;
  }
  if (!e) {
    return nullptr;
  } else {
    return getNext(events, e);
  }
}

bool is_event_visible(const Event *e) {
  assert(e);
  switch (e->t) {
    case E_MOVE: {
      return is_visible_event_move(&e->e.move);
    }
    case E_END_TURN: {
      return true;
    }
    default: {
      die("event: is_event_visible(): "
          "unknown event '%d'\n", e->t);
      return true;
    }
  }
}

/* TODO simplify */
void apply_invisible_events(void) {
  Event* e = get_next_event_node();
  while (e) {
    assert(e);
    if (!is_event_visible(e)) {
      apply_event(e);
    } else {
      break;
    }
    e = getNext(events, e);
    assert(e);
  }
}

/* TODO: Called before get_next_event */
bool unshown_events_left(void) {
  apply_invisible_events();
  if (events.size() == 0) {
    return false;
  } else {
    auto e = events.back();
    return e->id != current_player->last_event_id;
  }
}

/* Always called after apply_invisible_events */
Event* get_next_event(void) {
  int id = current_player->last_event_id; /* shortcut */
  assert(events.size() > 0);
  if (id == HAVE_NOT_SEEN_ANY_EVENTS) {
    return events.front();
  }
  for (auto e : events) {
    if (e->id == id) {
      return getNext(events, e);
    }
  }
  return NULL;
}

static int get_new_event_id(void) {
  if (events.size() > 0) {
    auto lastEvent = events.back();
    return lastEvent->id + 1;
  } else {
    return 0;
  }
}

static void event2log(const Event *e) {
  assert(e);
  UNUSED(e);
  /* TODO */
}

static void send_event(const Event *e) {
  assert(e);
  UNUSED(e);
  /* TODO */
}

void add_event(Event *e) {
  assert(e);
  e->id = get_new_event_id();
  events.push_back(e);
  event2log(e);
#if 0
  /* TODO */
  if (!is_local) {
    send_event(e);
  }
#else
  UNUSED(send_event);
#endif
}