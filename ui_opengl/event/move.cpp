/* See LICENSE file for copyright and license details. */

#include <cassert>
#include <SDL/SDL_opengl.h>
#include "core/misc.h"
#include "core/v2i.h"
#include "core/dir.h"
#include "core/core.h"
#include "core/path.h"
#include "ui_opengl/v2f.h"
#include "ui_opengl/vertex_array.h"
#include "ui_opengl/game.h"

static int get_move_legth(const V2i& from, const V2i& to) {
  if (Dir(from, to).isDiagonal()) {
    return 14;
  } else {
    return 10;
  }
}

int get_last_event_move_index(Game& game, const Event &e) {
  const EventMove* m = &e.e.move;
  auto& p = m->path; /* shortcut */
  int length = 0;
  int i;
  for (i = 1; i < m->length; i++) {
    length += get_move_legth(p[i - 1], p[i]);
  }
  return length;
}

void get_current_moving_nodes(
    Game& game, const EventMove& e, V2i* from, V2i* to)
{
  int i = game.current_move_index;
  auto& p = e.path; /* shortcut */
  unsigned int j; /* node id */
  assert(from);
  assert(to);
  for (j = 0; j < p.size() - 2; j++) {
    i -= get_move_legth(p[j], p[j + 1]);
    if (i < 0) {
      break;
    }
  }
  *from = p[j];
  *to = p[j + 1];
}

static void end_movement(Game& game, const EventMove& e, const V2i *pos) {
  Unit *u;
  assert(pos);
  u = game.core.id2unit(e.unit_id);
  game.ui_mode = UIMode::UI_MODE_NORMAL;
  u->pos = *pos;
  if (game.core.selected_unit) {
    game.core.pathfinder.fill_map(*u);
    game.build_walkable_array(&game.va_walkable_map);
    game.core.calculate_fow();
    game.build_fow_array(&game.va_fog_of_war);
  }
  apply_event(game.core, *game.core.current_event);
  game.core.current_event = nullptr;
  if (u->player_id == game.core.current_player->id) {
    if (game.core.selected_unit) {
      game.core.pathfinder.fill_map(*game.core.selected_unit);
      game.build_walkable_array(&game.va_walkable_map);
    }
    game.build_fow_array(&game.va_fog_of_war);
  }
}

static int get_node_index(Game& game, const EventMove& e) {
  auto& p = e.path; /* shortcut */
  int last = 0;
  int current = 0;
  for (unsigned int j = 0; j < p.size() - 2; j++) {
    current += get_move_legth(p[j], p[j + 1]);
    if (current > game.current_move_index) {
      break;
    }
    last = current;
  }
  return game.current_move_index - last;
}

void draw_moving_unit(Game& game, const EventMove& e) {
  Unit *u = game.core.id2unit(e.unit_id);
  V2i from_i, to_i;
  V2f diff;
  V2f p;
  get_current_moving_nodes(game, e, &from_i, &to_i);
  V2f from_f = game.v2i_to_v2f(from_i);
  V2f to_f = game.v2i_to_v2f(to_i);
  int move_speed = get_move_legth(from_i, to_i);
  int node_index = get_node_index(game, e);
  diff.setX((to_f.x() - from_f.x()) / move_speed);
  diff.setY((to_f.y() - from_f.y()) / move_speed);
  p.setX(from_f.x() + diff.x() * node_index);
  p.setY(from_f.y() + diff.y() * node_index);
  glPushMatrix();
  glTranslatef(p.x(), p.y(), 0.0f);
  /* TODO: Remove '+ 4'! Rotate obj files! */
  glRotatef(
      (Dir(from_i, to_i).toInt() + 4) * 45.0f,
      0, 0, 1);
  game.draw_unit_model(u);
  game.draw_unit_circle(u);
  glPopMatrix();
  game.current_move_index++;
  if (game.current_move_index == game.last_move_index) {
    end_movement(game, e, &to_i);
  }
}
