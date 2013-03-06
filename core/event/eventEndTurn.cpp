// See LICENSE file for copyright and license details.

#include "core/event/eventEndTurn.hpp"
#include <cassert>
#include <cstdio>
#include "core/core.hpp"
#include "core/player.hpp"

EventEndTurn::EventEndTurn(int oldPlayerID, int newPlayerID)
  : Event(EventType::EndTurn),
    mOldID(oldPlayerID),
    mNewID(newPlayerID)
{
}

EventEndTurn::~EventEndTurn() {
}

int EventEndTurn::newID() const {
  return mNewID;
}

int EventEndTurn::oldID() const {
  return mOldID;
}

void EventEndTurn::apply(Core& core) const {
  core.deselectedAnyUnits();
  core.cleanFow();
  core.refreshUnits(newID());
  for (auto* p : core.players()) {
    if (p->id == newID()) {
      if (core.currentPlayer().id == oldID()) {
        core.setCurrentPlayer(p);
      }
      return;
    }
  }
}

bool EventEndTurn::isVisible(const Core& core) const {
  UNUSED(core);
  return true;
}

EventEndTurn* EventEndTurn::generate(const Core& core) {
  unsigned int newPlayerID = core.currentPlayer().id + 1;
  if (newPlayerID == core.players().size()) {
    newPlayerID = 0;
  }
  int oldPlayerID = core.currentPlayer().id;
  auto* e = new EventEndTurn(
      oldPlayerID, newPlayerID);
  return e;
}
