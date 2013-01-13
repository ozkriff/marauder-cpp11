// See LICENSE file for copyright and license details.

#include "core/event/end_turn.hpp"
#include <cassert>
#include <cstdio>
#include "core/core.hpp"

EventEndTurn::EventEndTurn(int id)
  : Event(id, EventTypeID::END_TURN)
{
}

EventEndTurn::~EventEndTurn() {
}

void EventEndTurn::apply(Core& core) {
  for (auto p : core.players()) {
    if (p->id == newID) {
      if (core.currentPlayer().id == oldID) {
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
  auto e = new EventEndTurn(core.getNewEventID());
  int playersCount = 2; // TODO
  int newID = core.currentPlayer().id + 1;
  if (newID == playersCount) {
    newID = 0;
  }
  e->oldID = core.currentPlayer().id;
  e->newID = newID;
  core.addEvent(e);
}
