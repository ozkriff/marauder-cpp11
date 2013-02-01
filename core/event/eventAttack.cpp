// See LICENSE file for copyright and license details.

#include <cassert>
#include "core/event/eventAttack.hpp"
#include "core/core.hpp"
#include "core/misc.hpp"

EventAttack::EventAttack(int id)
  : Event(id, EventTypeID::ATTACK)
{
}

EventAttack::~EventAttack() {
}

void EventAttack::apply(Core& core) {
  Unit& u = core.id2unit(mVictimID);
  core.deadUnits().push_back(&u);
  core.units().remove(&u);
  if (core.isAnyUnitSelected()) {
    core.pathfinder().fillMap(core.selectedUnit());
    core.calculateFow();
  }
}

void EventAttack::undo(Core& core) {
  assert(!core.deadUnits().empty());
  Unit* u = core.deadUnits().back();
  assert(u);
  core.deadUnits().remove(u);
  core.units().push_back(u);
  if (core.isAnyUnitSelected()) {
    core.pathfinder().fillMap(core.selectedUnit());
    core.calculateFow();
  }
}

bool EventAttack::isVisible(const Core &core) const {
  UNUSED(core);
  return true;
}

void EventAttack::generate(
    Core& core, const Unit& u, const Unit& otherUnit)
{
  auto event = new EventAttack(core.getNewEventID());
  if (!core.isLosClear(u.pos, otherUnit.pos)) {
    return;
  }
  event->mAttackerID = u.id;
  event->mVictimID = otherUnit.id;
  core.addEvent(event);
}
