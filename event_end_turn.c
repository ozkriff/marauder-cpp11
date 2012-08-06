/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include <stdio.h>
#include "bool.h"
#include "list.h"
#include "misc.h"
#include "v2i.h"
#include "dir.h"
#include "unit_type.h"
#include "core.h"
#include "event_end_turn.h"
#include "event_move.h"
#include "event.h"
#include "path.h"
#include "core_private.h"

void apply_event_end_turn(const EventEndturn *e) {
  Node *nd;
  assert(e);
  FOR_EACH_NODE(players, nd) {
    Player *p = nd->data;
    if (p->id == e->new_id) {
      if (current_player->id == e->old_id) {
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

void generate_event_end_turn(void) {
  Event *e = ALLOCATE(1, Event);
  int players_count = 2; /* TODO */
  int new_id = current_player->id + 1;
  if (new_id == players_count) {
    new_id = 0;
  }
  e->t = E_END_TURN;
  e->e.end_turn.old_id = current_player->id;
  e->e.end_turn.new_id = new_id;
  add_event(e);
}
