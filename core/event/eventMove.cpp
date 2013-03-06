// See LICENSE file for copyright and license details.

#include "core/event/eventMove.hpp"
#include <cassert>
#include "core/core.hpp"
#include "core/player.hpp"

EventMove::EventMove()
  : Event(EventType::Move),
    mUnitID(0),
    mCost(0)
{
}

EventMove::~EventMove() {
}

UnitID EventMove::unitID() const {
  return mUnitID;
}

const std::vector<V2i>& EventMove::path() const {
  return mPath;
}

EventMove* EventMove::generate(
    const Core& core, const Unit& unit, const V2i& destination)
{
  auto* e = new EventMove();
  e->mInitialDirection = unit.direction();
  e->mPath = core.pathfinder().getPath(destination);
  e->mCost = core.map().tile(destination).cost;
  e->mUnitID = unit.id();
  return e;
}

void EventMove::apply(Core& core) const {
  Unit& u = core.id2unit(mUnitID);
  if (u.playerID() == core.currentPlayer().id) {
    for (const V2i& p : mPath) {
      u.setPosition(p);
      // TODO: u.setDirection?
      core.calculateFow();
    }
  }
  u.setPosition(mPath[mPath.size() - 1]);
  u.setDirection(Dir(mPath[mPath.size() - 2], mPath[mPath.size() - 1]));
  u.setActionPoints(u.actionPoints() - mCost);
  if (core.isAnyUnitSelected()) {
    core.pathfinder().fillMap(core.selectedUnit());
    core.calculateFow();
  }
}

bool EventMove::isVisible(const Core &core) const {
  for (const V2i& pos : mPath) {
    if (core.map().tile(pos).fow > 0) {
      return true;
    }
  }
  return false;
}
