// See LICENSE file for copyright and license details.

#include "core/event/end_turn.hpp"
#include <cassert>
#include <cstdio>
#include "core/core.hpp"

void EventEndturn::apply(Core& core) {
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

void EventEndturn::undo(Core& core) {
  UNUSED(core);
}

bool EventEndturn::isVisible(const Core& core) const {
  UNUSED(core);
  return true;
}
  
void generateEventEndTurn(Core& core) {
  auto e = new EventEndturn;
  int playersCount = 2; // TODO
  int newID = core.currentPlayer().id + 1;
  if (newID == playersCount) {
    newID = 0;
  }
  e->t = EventTypeID::END_TURN;
  e->oldID = core.currentPlayer().id;
  e->newID = newID;
  core.addEvent(e);
}
