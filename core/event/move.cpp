/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include "core/core.h"

void generate_event_move(
    const Unit *u, const V2i *destination)
{
  Event *e = new Event;
  EventMove *m = &e->e.move;
  int ap = get_unit_type(u->type_id).action_points;
  if (tile(*destination).cost > ap) {
    return;
  }
  e->t = EventTypeId::E_MOVE;
  m->initial_direction = u->dir;
  m->length = get_path_length(*destination);
  m->path = new V2i[m->length];
  get_path(m->path, m->length, *destination);
  m->unit_id = u->id;
  add_event(e);
}

void apply_event_move(const EventMove& e) {
  const V2i* p = e.path; /* shortcut */
  Unit *u = id2unit(e.unit_id);
  assert(u);
  u->pos = p[e.length - 1];
  u->dir = m2dir(p[e.length - 2], p[e.length - 1]);
  if (u->player_id == current_player->id) {
    calculate_fow();
  }
}

void undo_event_move(const EventMove& e) {
  Unit *u = id2unit(e.unit_id);
  u->pos = e.path[0];
  u->dir = e.initial_direction;
}

bool is_visible_event_move(const EventMove& e) {
  UNUSED(e);
  /* TODO */
  return true;
}
