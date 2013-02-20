// See LICENSE file for copyright and license details.

#include "core/event/eventMove.hpp"
#include <cassert>
#include "core/core.hpp"

EventMove::EventMove(int id)
  : Event(id, EventTypeID::MOVE),
    mUnitID(0),
    mCost(0)
{
}

EventMove::~EventMove() {
}

int EventMove::unitID() const {
  return mUnitID;
}

const std::vector<V2i>& EventMove::path() const {
  return mPath;
}

EventMove* EventMove::generate(
    Core& core, const Unit& unit, const V2i& destination)
{
  auto e = new EventMove(core.eventManager().getNewEventID());
  e->mInitialDirection = unit.direction();
  e->mPath = core.pathfinder().getPath(destination);
  e->mCost = core.map().tile(destination).cost;
  e->mUnitID = unit.id();
  return e;
}

void EventMove::apply(Core& core) const {
  Unit& u = core.id2unit(mUnitID);
  u.setPosition(mPath[mPath.size() - 1]);
  u.setDirection(Dir(mPath[mPath.size() - 2], mPath[mPath.size() - 1]));
  u.setActionPoints(u.actionPoints() - mCost);
  if (u.playerID() == core.currentPlayer().id) {
    core.calculateFow();
  }
  if (core.isAnyUnitSelected()) {
    core.pathfinder().fillMap(core.selectedUnit());
    core.calculateFow();
  }
}

void EventMove::undo(Core& core) const {
  Unit& u = core.id2unit(mUnitID);
  u.setPosition(mPath[0]);
  u.setDirection(mInitialDirection);
  u.setActionPoints(u.actionPoints() + mCost);
}

bool EventMove::isVisible(const Core &core) const {
  for (const V2i& pos : mPath) {
    if (core.map().tile(pos).fow > 0) {
      return true;
    }
  }
  return false;
}
