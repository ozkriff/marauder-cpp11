// See LICENSE file for copyright and license details.

#include "core/event/eventMove.hpp"
#include <cassert>
#include "core/core.hpp"

EventMove::EventMove(int id)
  : Event(id, EventTypeID::MOVE),
    unitID(0),
    cost(0)
{
}

EventMove::~EventMove() {
}

void generateEventMove(
    Core& core, const Unit& unit, const V2i& destination)
{
  auto e = new EventMove(core.eventManager().getNewEventID());
  int ap = getUnitType(unit.typeID).actionPoints;
  if (core.map().tile(destination).cost > ap) {
    return;
  }
  e->initialDirection = unit.dir;
  e->path = core.pathfinder().getPath(destination);
  e->cost = core.map().tile(destination).cost;
  e->unitID = unit.id;
  core.eventManager().addEvent(e);
}

void EventMove::apply(Core& core) {
  Unit& u = core.id2unit(unitID);
  u.pos = path[path.size() - 1];
  u.dir = Dir(path[path.size() - 2], path[path.size() - 1]);
  u.actionPoints -= cost;
  if (u.playerID == core.currentPlayer().id) {
    core.calculateFow();
  }
}

void EventMove::undo(Core& core) {
  Unit& u = core.id2unit(unitID);
  u.pos = path[0];
  u.dir = initialDirection;
  u.actionPoints += cost;
}

bool EventMove::isVisible(const Core &core) const {
  for (const V2i& pos : path) {
    if (core.map().tile(pos).fow > 0) {
      return true;
    }
  }
  return false;
}
