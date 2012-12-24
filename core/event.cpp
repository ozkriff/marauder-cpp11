/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include <stdlib.h>
#include "core/list.h"
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

static List events;

void init_events(void) {
  events = empty_list;
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
  Node *node = events.tail;
  while (node) {
    Event *e = (Event *)node->data;
    if (e->id == current_player->last_event_id) {
      break;
    }
    undo_event(e);
    node = node->prev;
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
static Node* get_next_event_node(void) {
  Node *node;
  int id = current_player->last_event_id; /* shortcut */
  if (events.count == 0) {
    return NULL;
  }
  if (id == HAVE_NOT_SEEN_ANY_EVENTS) {
    return events.head;
  }
  /* find last seen event */
  FOR_EACH_NODE(events, node) {
    Event *e = (Event *)node->data;
    if (e->id == id)
      break;
  }
  if (!node) {
    return NULL;
  } else {
    return node->next;
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
  Node *node = get_next_event_node();
  while (node) {
    Event *e = (Event *)node->data;
    if (!is_event_visible(e)) {
      apply_event(e);
    } else {
      break;
    }
    node = node->next;
  }
}

/* TODO: Called before get_next_event */
bool unshown_events_left(void) {
  apply_invisible_events();
  if (events.count == 0) {
    return false;
  } else {
    Event *e = (Event *)events.tail->data;
    return e->id != current_player->last_event_id;
  }
}

/* Always called after apply_invisible_events */
Event* get_next_event(void) {
  int id = current_player->last_event_id; /* shortcut */
  Node *node;
  assert(events.count > 0);
  if (id == HAVE_NOT_SEEN_ANY_EVENTS) {
    return CAST(events.head->data, Event*);
  }
  FOR_EACH_NODE(events, node) {
    if (CAST(node->data, Event*)->id == id) {
      return CAST(node->next->data, Event*);
    }
  }
  return NULL;
}

static int get_new_event_id(void) {
  if (events.count > 0) {
    Event *last = (Event *)events.tail->data;
    return last->id + 1;
  } else {
    return 0;
  }
}

static void add_event_local(Event *data) {
  Node *n = new Node;
  set_node(n, data);
  add_node_to_tail(&events, n);
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
  add_event_local(e);
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
