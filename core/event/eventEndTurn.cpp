// See LICENSE file for copyright and license details.

#include "core/event/eventEndTurn.hpp"
#include <cassert>
#include <cstdio>
#include "core/core.hpp"
#include "core/player.hpp"

EventEndTurn::EventEndTurn(PlayerID oldPlayerID, PlayerID newPlayerID)
  : Event(EventType::EndTurn),
    mOldID(oldPlayerID),
    mNewID(newPlayerID)
{
}

EventEndTurn::~EventEndTurn() {
}

PlayerID EventEndTurn::newID() const {
  return mNewID;
}

PlayerID EventEndTurn::oldID() const {
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
  PlayerID newPlayerID = core.currentPlayer().id + 1;
  if (newPlayerID == static_cast<PlayerID>(core.players().size())) {
    newPlayerID = 0;
  }
  PlayerID oldPlayerID = core.currentPlayer().id;
  auto* e = new EventEndTurn(oldPlayerID, newPlayerID);
  return e;
}
