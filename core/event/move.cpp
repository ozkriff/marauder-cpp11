// See LICENSE file for copyright and license details.

#include <assert.h>
#include "core/core.h"

EventMove::EventMove() {
}

EventMove::~EventMove() {
}

void generate_event_move(
    Core& core, const Unit &u, const V2i &destination)
{
  Event *e = new Event;
  EventMove *m = &e->e.move;
  int ap = get_unit_type(u.type_id).action_points;
  if (core.tile(destination).cost > ap) {
    return;
  }
  e->t = EventTypeId::E_MOVE;
  m->initial_direction = u.dir;
  m->path = core.pathfinder.get_path(destination);
  m->length = m->path.size();
  m->unit_id = u.id;
  add_event(core, e);
}

void apply_event_move(Core& core, const EventMove& e) {
  auto& p = e.path; // shortcut
  Unit *u = core.id2unit(e.unit_id);
  assert(u);
  u->pos = p[e.length - 1];
  u->dir = Dir(p[e.length - 2], p[e.length - 1]);
  if (u->player_id == core.current_player->id) {
    core.calculate_fow();
  }
}

void undo_event_move(Core& core, const EventMove& e) {
  Unit *u = core.id2unit(e.unit_id);
  u->pos = e.path[0];
  u->dir = e.initial_direction;
}

bool is_visible_event_move(const Core &core, const EventMove &e) {
  UNUSED(e);
  // TODO
  return true;
}
