// See LICENSE file for copyright and license details.

#include "core/event/move.h"
#include <cassert>
#include "core/core.h"

void generateEventMove(
    Core& core, const Unit& unit, const V2i& destination)
{
  auto e = new EventMove;
  int ap = getUnitType(unit.typeID).actionPoints;
  if (core.tile(destination).cost > ap) {
    return;
  }
  e->t = EventTypeID::MOVE;
  e->initialDirection = unit.dir;
  e->path = core.pathfinder().getPath(destination);
  e->cost = core.tile(destination).cost;
  e->unitID = unit.id;
  core.addEvent(e);
}

void EventMove::apply(Core& core) {
  Unit* u = core.id2unit(unitID);
  assert(u);
  u->pos = path[path.size() - 1];
  u->dir = Dir(path[path.size() - 2], path[path.size() - 1]);
  u->actionPoints -= cost;
  if (u->playerID == core.currentPlayer().id) {
    core.calculateFow();
  }
}

void EventMove::undo(Core& core) {
  Unit* u = core.id2unit(unitID);
  assert(u);
  u->pos = path[0];
  u->dir = initialDirection;
  u->actionPoints += cost;
}

bool EventMove::isVisible(const Core &core) const {
  UNUSED(core);
  // TODO
  return true;
}
