// See LICENSE file for copyright and license details.

#include <cassert>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include "core/v2i.h"
#include "core/dir.h"
#include "core/core.h"
#include "core/misc.h"
#include "core/event.h"
#include "ui_opengl/v2f.h"
#include "ui_opengl/v3f.h"
#include "ui_opengl/math.h"
#include "ui_opengl/vertex_array.h"
#include "ui_opengl/event.h"
#include "ui_opengl/event/move.h"

bool eventFilterUnit(Game& game, const Event& e, const Unit& u) {
  switch (e.t) {
  case EventTypeID::END_TURN:
    return false;
  case EventTypeID::MOVE:
    return u.id == e.e.move.unitID;
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
    drawMovingUnit(game, e.e.move);
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
