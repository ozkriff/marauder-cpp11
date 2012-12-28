// See LICENSE file for copyright and license details.

#include <cassert>
#include <cstdio>
#include "core/core.h"

void applyEventEndTurn(Core& core, const EventEndturn& e) {
  for (auto p : core.players) {
    if (p->id == e.newID) {
      if (core.currentPlayer->id == e.oldID) {
        core.currentPlayer = p;
        core.undoUnshownEvents();
      } else {
        // refreshUnits(currentPlayer->id);
      }
      return;
    }
  }
}

void generateEventEndTurn(Core& core) {
  Event* e = new Event;
  int playersCount = 2; // TODO
  int newID = core.currentPlayer->id + 1;
  if (newID == playersCount) {
    newID = 0;
  }
  e->t = EventTypeID::END_TURN;
  e->e.endTurn.oldID = core.currentPlayer->id;
  e->e.endTurn.newID = newID;
  core.addEvent(e);
}
