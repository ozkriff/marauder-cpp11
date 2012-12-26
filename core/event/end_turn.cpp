/* See LICENSE file for copyright and license details. */

#include <cassert>
#include <cstdio>
#include "core/core.h"

void apply_event_end_turn(const EventEndturn& e) {
  for (auto p : players) {
    if (p->id == e.new_id) {
      if (current_player->id == e.old_id) {
        current_player = p;
        undo_unshown_events();
      } else {
#if 0
        refresh_units(current_player->id);
#endif
      }
      return;
    }
  }
}

void generate_event_end_turn() {
  Event *e = new Event;
  int players_count = 2; /* TODO */
  int new_id = current_player->id + 1;
  if (new_id == players_count) {
    new_id = 0;
  }
  e->t = EventTypeId::E_END_TURN;
  e->e.end_turn.old_id = current_player->id;
  e->e.end_turn.new_id = new_id;
  add_event(e);
}
