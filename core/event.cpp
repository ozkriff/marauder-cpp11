/* See LICENSE file for copyright and license details. */

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

void init_events() {
}

static void undo_event(Core& core, const Event& e) {
  switch (e.t) {
    case EventTypeId::E_MOVE: {
      undo_event_move(core, e.e.move);
      break;
    }
    case EventTypeId::E_END_TURN: {
      /* empty */
      /* TODO: die()? */
      break;
    }
    default:
      die("event: undo_event(): "
          "unknown event type '%d'\n", e.t);
      break;
  }
  /* update_units_visibility(); */
}

/* Undo all events that this player have not seen yet */
void undo_unshown_events(Core& core) {
  if (core.events.size() == 0) {
    return;
  }
  auto i = core.events.end();
  --i;
  while (i != core.events.begin()) {
    auto event = *i;
    if (event->id == game.core.current_player->last_event_id) {
      break;
    }
    undo_event(core, *event);
    --i;
  }
}

void apply_event(Core& core, const Event& e) {
  game.core.current_player->last_event_id = e.id;
  switch (e.t) {
  case EventTypeId::E_MOVE:
    apply_event_move(core, e.e.move);
    break;
  case EventTypeId::E_END_TURN:
    apply_event_end_turn(core, e.e.end_turn);
    break;
  default:
    die("event: apply_event(): "
        "unknown event '%d'\n", e.t);
    break;
  }
  /* update_units_visibility(); */
}

/* TODO: rename. */
static Event* get_next_event_node(Core& core) {
  // Node *node;
  int id = game.core.current_player->last_event_id; /* shortcut */
  if (core.events.size() == 0) {
    return nullptr;
  }
  if (id == HAVE_NOT_SEEN_ANY_EVENTS) {
    return core.events.front();
  }
  /* find last seen event */
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

bool is_event_visible(const Core& core, const Event& e) {
  switch (e.t) {
    case EventTypeId::E_MOVE: {
      return is_visible_event_move(core, e.e.move);
    }
    case EventTypeId::E_END_TURN: {
      return true;
    }
    default: {
      die("event: is_event_visible(): "
          "unknown event '%d'\n", e.t);
      return true;
    }
  }
}

/* TODO simplify */
void apply_invisible_events(Core& core) {
  Event* e = get_next_event_node(core);
  while (e) {
    assert(e);
    if (!is_event_visible(core, *e)) {
      apply_event(core, *e);
    } else {
      break;
    }
    e = getNext(core.events, e);
    assert(e);
  }
}

/* TODO: Called before get_next_event */
bool unshown_events_left(Core& core) {
  apply_invisible_events(core);
  if (core.events.size() == 0) {
    return false;
  } else {
    auto e = core.events.back();
    return e->id != game.core.current_player->last_event_id;
  }
}

/* Always called after apply_invisible_events */
Event* get_next_event(Core& core) {
  int id = game.core.current_player->last_event_id; /* shortcut */
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

static int get_new_event_id(Core& core) {
  if (core.events.size() > 0) {
    auto lastEvent = core.events.back();
    return lastEvent->id + 1;
  } else {
    return 0;
  }
}

static void event2log(const Event& e) {
  UNUSED(e);
  /* TODO */
}

static void send_event(const Event& e) {
  UNUSED(e);
  /* TODO */
}

void add_event(Core& core, Event* e) {
  assert(e);
  e->id = get_new_event_id(core);
  core.events.push_back(e);
  event2log(*e);
#if 0
  /* TODO */
  if (!is_local) {
    send_event(e);
  }
#else
  UNUSED(send_event);
#endif
}
