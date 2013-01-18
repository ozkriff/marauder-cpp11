// See LICENSE file for copyright and license details.

#include "core/event/eventEndTurn.hpp"
#include <cassert>
#include <cstdio>
#include "core/core.hpp"

EventEndTurn::EventEndTurn(int eventID, int oldPlayerID, int newPlayerID)
  : Event(eventID, EventTypeID::END_TURN),
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

void EventEndTurn::apply(Core& core) {
  for (auto p : core.players()) {
    if (p->id == newID()) {
      if (core.currentPlayer().id == oldID()) {
        core.setCurrentPlayer(p);
        core.undoUnshownEvents();
      } else {
        core.refreshUnits(core.currentPlayer().id);
      }
      return;
    }
  }
}

void EventEndTurn::undo(Core& core) {
  UNUSED(core);
}

bool EventEndTurn::isVisible(const Core& core) const {
  UNUSED(core);
  return true;
}

void generateEventEndTurn(Core& core) {
  unsigned int newPlayerID = core.currentPlayer().id + 1;
  if (newPlayerID == core.players().size()) {
    newPlayerID = 0;
  }
  int oldPlayerID = core.currentPlayer().id;
  auto e = new EventEndTurn(core.getNewEventID(), oldPlayerID, newPlayerID);
  core.addEvent(e);
}