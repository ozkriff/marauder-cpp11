// See LICENSE file for copyright and license details.

#include <cassert>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include "core/v2i.hpp"
#include "core/dir.hpp"
#include "core/core.hpp"
#include "core/misc.hpp"
#include "core/event.hpp"
#include "ui/v2f.hpp"
#include "ui/v3f.hpp"
#include "ui/math.hpp"
#include "ui/vertex_array.hpp"
#include "ui/event.hpp"
#include "ui/event/move.hpp"

bool eventFilterUnit(Game& game, const Event& e, const Unit& u) {
  UNUSED(game);
  switch (e.t) {
  case EventTypeID::END_TURN:
    return false;
  case EventTypeID::MOVE: {
    auto eventMove = dynamic_cast<const EventMove*>(&e);
    return u.id == eventMove->unitID;
  }
  default:
    die("uiEvent: eventFilterUnit(): "
        "unknow event '%d'.\n", e.t);
    return false;
  }
}

void eventDraw(Game& game, const Event& e) {
  switch (e.t) {
  case EventTypeID::END_TURN: {
    die("TODO");
    break;
  }
  case EventTypeID::MOVE: {
    auto eventMove = dynamic_cast<const EventMove*>(&e);
    drawMovingUnit(game, *eventMove);
    break;
  }
  default:
    die("uiEvent: eventDraw(): "
        "unknow event '%d'.\n", e.t);
    break;
  }
}

int getLastEventIndex(Game& game, const Event& e) {
  switch (e.t) {
  case EventTypeID::END_TURN: {
    return 0;
  }
  case EventTypeID::MOVE: {
    return getLastEventMoveIndex(game, e);
  }
  default:
    die("uiEvent: getLastEventIndex(): "
        "unknow event '%d'.\n", e.t);
    return 0;
  }
}
