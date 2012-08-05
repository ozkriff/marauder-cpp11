/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include "bool.h"
#include "list.h"
#include "v2i.h"
#include "v3f.h"
#include "v2f.h"
#include "math.h"
#include "dir.h"
#include "va.h"
#include "misc.h"
#include "game.h"
#include "event_move.h"
#include "event_end_turn.h"
#include "event.h"
#include "game_private.h"
#include "ui_event_move.h"

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
