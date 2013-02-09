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

void EventMove::generate(
    Core& core, const Unit& unit, const V2i& destination)
{
  auto e = new EventMove(core.eventManager().getNewEventID());
  int ap = getUnitType(unit.typeID()).actionPoints;
  if (core.map().tile(destination).cost > ap) {
    return;
  }
  e->initialDirection = unit.direction();
  e->path = core.pathfinder().getPath(destination);
  e->cost = core.map().tile(destination).cost;
  e->unitID = unit.id();
  core.eventManager().addEvent(e);
}

void EventMove::apply(Core& core) const {
  Unit& u = core.id2unit(unitID);
  u.setPosition(path[path.size() - 1]);
  u.setDirection(Dir(path[path.size() - 2], path[path.size() - 1]));
  u.setActionPoints(u.actionPoints() - cost);
  if (u.playerID() == core.currentPlayer().id) {
    core.calculateFow();
  }
}

void EventMove::undo(Core& core) const {
  Unit& u = core.id2unit(unitID);
  u.setPosition(path[0]);
  u.setDirection(initialDirection);
  u.setActionPoints(u.actionPoints() + cost);
}

bool EventMove::isVisible(const Core &core) const {
  for (const V2i& pos : path) {
    if (core.map().tile(pos).fow > 0) {
      return true;
    }
  }
  return false;
}
