/* See LICENSE file for copyright and license details. */

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
#include "ui_opengl/event/move.h"

bool event_filter_unit(const Event *e, const Unit *u) {
  assert(e);
  assert(u);
  switch (e->t) {
  case EventTypeId::E_END_TURN: {
    return false;
  }
  case EventTypeId::E_MOVE: {
    return u->id == current_event->e.move.unit_id;
  }
  default:
    die("ui_event: event_filter_unit(): "
        "unknow event '%d'.\n", e->t);
    return false;
  }
}

void event_draw(const Event *e) {
  assert(e);
  switch (e->t) {
  case EventTypeId::E_END_TURN: {
    die("TODO");
    break;
  }
  case EventTypeId::E_MOVE: {
    draw_moving_unit();
    break;
  }
  default:
    die("ui_event: event_draw(): "
        "unknow event '%d'.\n", e->t);
    break;
  }
}

int get_last_event_index(const Event *e) {
  assert(e);
  switch (e->t) {
  case EventTypeId::E_END_TURN: {
    return 0;
  }
  case EventTypeId::E_MOVE: {
    return get_last_event_move_index(e);
  }
  default:
    die("ui_event: get_last_event_index(): "
        "unknow event '%d'.\n", e->t);
    return 0;
  }
}
