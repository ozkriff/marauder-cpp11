// See LICENSE file for copyright and license details.

#include <assert.h>
#include "core/core.h"

EventMove::EventMove() {
}

EventMove::~EventMove() {
}

void generateEventMove(
    Core& core, const Unit &u, const V2i &destination)
{
  Event *e = new Event;
  EventMove *m = &e->e.move;
  int ap = getUnitType(u.typeID).actionPoints;
  if (core.tile(destination).cost > ap) {
    return;
  }
  e->t = EventTypeID::MOVE;
  m->initialDirection = u.dir;
  m->path = core.pathfinder.getPath(destination);
  m->length = m->path.size();
  m->unitID = u.id;
  addEvent(core, e);
}

void applyEventMove(Core& core, const EventMove& e) {
  auto& p = e.path; // shortcut
  Unit *u = core.id2unit(e.unitID);
  assert(u);
  u->pos = p[e.length - 1];
  u->dir = Dir(p[e.length - 2], p[e.length - 1]);
  if (u->playerID == core.currentPlayer->id) {
    core.calculateFow();
  }
}

void undoEventMove(Core& core, const EventMove& e) {
  Unit *u = core.id2unit(e.unitID);
  u->pos = e.path[0];
  u->dir = e.initialDirection;
}

bool isVisibleEventMove(const Core &core, const EventMove &e) {
  UNUSED(e);
  // TODO
  return true;
}
