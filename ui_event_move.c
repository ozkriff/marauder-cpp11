/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include <SDL/SDL_opengl.h>
#include "bool.h"
#include "list.h"
#include "misc.h"
#include "v2i.h"
#include "v2f.h"
#include "dir.h"
#include "unit_type.h"
#include "core.h"
#include "event_move.h"
#include "event_end_turn.h"
#include "event.h"
#include "path.h"
#include "va.h"
#include "core_private.h"
#include "ui_game_private.h"

static int get_move_legth(const V2i *from, const V2i *to) {
  Dir d = m2dir(from, to);
  if (dir_is_diagonal(d)) {
    return 14;
  } else {
    return 10;
  }
}

int get_last_event_move_index(const Event *e) {
  const EventMove *m = &e->e.move;
  const V2i *p = m->path; /* shortcut */
  int length = 0;
  int i;
  for (i = 1; i < m->length; i++) {
    length += get_move_legth(&p[i - 1], &p[i]);
  }
  return length;
}

void get_current_moving_nodes(V2i *from, V2i *to) {
  int i = current_move_index;
  const EventMove *m = &current_event->e.move;
  const V2i *p = m->path; /* shortcut */
  int j; /* node id */
  assert(from);
  assert(to);
  for (j = 0; j < m->length - 2; j++) {
    i -= get_move_legth(&p[j], &p[j + 1]);
    if (i < 0) {
      break;
    }
  }
  *from = m->path[j];
  *to = m->path[j + 1];
}

static void end_movement(const V2i *pos) {
  Unit *u;
  assert(pos);
  u = id2unit(current_event->e.move.unit_id);
  unit_mode = UM_NORMAL;
  u->pos = *pos;
  if (selected_unit) {
    fill_map(u);
    build_walkable_array(&va_walkable_map);
    calculate_fow();
    build_fow_array(&va_fog_of_war);
  }
  apply_event(current_event);
  current_event = NULL;
  if (u->player_id == current_player->id) {
    if (selected_unit) {
      fill_map(selected_unit);
      build_walkable_array(&va_walkable_map);
    }
    build_fow_array(&va_fog_of_war);
  }
}

static int get_node_index(void) {
  const EventMove *m = &current_event->e.move;
  const V2i *p = m->path; /* shortcut */
  int j;
  int last = 0;
  int current = 0;
  for (j = 0; j < m->length - 2; j++) {
    current += get_move_legth(&p[j], &p[j + 1]);
    if (current > current_move_index) {
      break;
    }
    last = current;
  }
  return current_move_index - last;
}

void draw_moving_unit(void) {
  Unit *u = id2unit(current_event->e.move.unit_id);
  V2i from_i, to_i;
  V2f from_f, to_f;
  int move_speed;
  int node_index;
  V2f diff;
  V2f p;
  get_current_moving_nodes(&from_i, &to_i);
  v2i_to_v2f(&from_f, &from_i);
  v2i_to_v2f(&to_f, &to_i);
  move_speed = get_move_legth(&from_i, &to_i);
  node_index = get_node_index();
  diff.x = (to_f.x - from_f.x) / move_speed;
  diff.y = (to_f.y - from_f.y) / move_speed;
  p.x = from_f.x + diff.x * node_index;
  p.y = from_f.y + diff.y * node_index;
  glPushMatrix();
  glTranslatef(p.x, p.y, 0.0f);
  /* TODO: Remove '+ 4'! Rotate obj files! */
  glRotatef((m2dir(&from_i, &to_i) + 4) * 45.0f, 0, 0, 1);
  draw_unit_model(u);
  draw_unit_circle(u);
  glPopMatrix();
  current_move_index++;
  if (current_move_index == last_move_index) {
    end_movement(&to_i);
  }
}
