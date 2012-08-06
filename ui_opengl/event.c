/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include "core/bool.h"
#include "core/list.h"
#include "core/v2i.h"
#include "core/dir.h"
#include "core/core.h"
#include "core/event/move.h"
#include "core/event/end_turn.h"
#include "core/misc.h"
#include "core/event.h"
#include "core/core_private.h"
#include "ui_opengl/v2f.h"
#include "ui_opengl/v3f.h"
#include "ui_opengl/math.h"
#include "ui_opengl/vertex_array.h"
#include "ui_opengl/event/move.h"

bool event_filter_unit(const Event *e, const Unit *u) {
  assert(e);
  assert(u);
  switch (e->t) {
    case E_END_TURN: {
      return false;
    }
    case E_MOVE: {
      return u->id == current_event->e.move.unit_id;
    }
    default: {
      die("ui_event: event_filter_unit(): "
          "unknow event '%d'.\n", e->t);
      return false;
    }
  }
}

void event_draw(const Event *e) {
  assert(e);
  switch (e->t) {
    case E_END_TURN: {
      die("TODO");
      break;
    }
    case E_MOVE: {
      draw_moving_unit();
      break;
    }
    default: {
      die("ui_event: event_draw(): "
          "unknow event '%d'.\n", e->t);
      break;
    }
  }
}

int get_last_event_index(const Event *e) {
  assert(e);
  switch (e->t) {
    case E_END_TURN: {
      return 0;
    }
    case E_MOVE: {
      return get_last_event_move_index(e);
    }
    default: {
      die("ui_event: get_last_event_index(): "
          "unknow event '%d'.\n", e->t);
      return 0;
    }
  }
}
