// See LICENSE file for copyright and license details.

#include <cassert>
#include "core/event/eventAttack.hpp"
#include "core/core.hpp"

EventAttack::EventAttack(int id)
  : Event(id, EventType::Attack)
{
}

EventAttack::~EventAttack() {
}

int EventAttack::victimID() const {
  return mVictimID;
}

int EventAttack::attackerID() const {
  return mAttackerID;
}

void EventAttack::apply(Core& core) const {
  Unit& u = core.id2unit(mVictimID);
  core.deadUnits().push_back(&u);
  core.units().remove(&u);
  Unit& attacker = core.id2unit(attackerID());
  attacker.setActionPoints(attacker.actionPoints() - 3);
  if (core.isAnyUnitSelected()) {
    core.pathfinder().fillMap(core.selectedUnit());
    core.calculateFow();
  }
}

void EventAttack::undo(Core& core) const {
  assert(!core.deadUnits().empty());
  Unit* u = core.deadUnits().back();
  assert(u);
  core.deadUnits().remove(u);
  core.units().push_back(u);
  Unit& attacker = core.id2unit(attackerID());
  attacker.setActionPoints(attacker.actionPoints() + 3);
  if (core.isAnyUnitSelected()) {
    core.pathfinder().fillMap(core.selectedUnit());
    core.calculateFow();
  }
}

bool EventAttack::isVisible(const Core &core) const {
  UNUSED(core);
  return true;
}

EventAttack* EventAttack::generate(
    const Core& core, const Unit& u, const Unit& otherUnit)
{
  auto* event = new EventAttack(core.eventManager().getNewEventID());
  event->mAttackerID = u.id();
  event->mVictimID = otherUnit.id();
  return event;
}
