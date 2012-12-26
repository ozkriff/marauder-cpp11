/* See LICENSE file for copyright and license details. */

#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cassert>
#include <ctime>
#include "core/core.h"

Core::Core()
  : pathfinder(*this)
{
}

Core::~Core() {
}

void Core::create_local_human(int id) {
  auto p = new Player;
  players.push_back(p);
  p->id = id;
  p->last_event_id = HAVE_NOT_SEEN_ANY_EVENTS;
}

void Core::init_local_players(int n, int* ids) {
  for (int i = 0; i < n; i++) {
    create_local_human(ids[i]);
  }
  current_player = players.back();
}

bool Core::inboard(const V2i& p) const {
  return p.x >= 0 && p.y >= 0
      && p.x < MAP_X && p.y < MAP_Y;
}

Tile& Core::tile(const V2i &p) {
  assert(inboard(p));
  return map[p.y][p.x];
}

void Core::inc_v2i(V2i *pos) const {
  assert(pos);
  assert(inboard(*pos));
  pos->x++;
  if (pos->x == MAP_X) {
    pos->x = 0;
    pos->y++;
  }
}

bool Core::isLosClear(const V2i& from, const V2i& to) {
  Los los(from, to);
  for (V2i p = los.getNext(); !los.isFinished(); p = los.getNext()) {
    if (unit_at(p) || tile(p).obstacle) {
      return false;
    }
  }
  return true;
}

#define FOR_EACH_TILE(p) \
  for (*p = V2i(0, 0); inboard(*p); inc_v2i(p))

void Core::clean_fow() {
  V2i p;
  FOR_EACH_TILE(&p) {
    Tile& t = tile(p);
    t.fow = 0;
  }
}

void Core::calculate_fow() {
  V2i p;
  assert(current_player);
  clean_fow();
  FOR_EACH_TILE(&p) {
    Tile& t = tile(p);
    for (auto u : units) {
      int max_dist = get_unit_type(u->type_id).range_of_vision;
      bool is_player_ok = (u->player_id == current_player->id);
      bool is_distance_ok = (dist(p, u->pos) < max_dist);
      bool is_los_ok = isLosClear(p, u->pos);
      if (is_player_ok && is_distance_ok && is_los_ok) {
        t.fow++;
      }
    }
  }
}

Unit* Core::unit_at(const V2i& pos) {
  for (auto u : units) {
    if (u->pos == pos) {
      return u;
    }
  }
  return nullptr;
}

Unit* Core::id2unit(int id) {
  for (auto u : units) {
    if (u->id == id) {
      return u;
    }
  }
  return nullptr;
}

int Core::get_new_unit_id() {
  if (units.size() > 0) {
    auto lastUnit = units.back();
    return lastUnit->id + 1;
  } else {
    return 0;
  }
}

void Core::add_unit(const V2i& p, int player_id) {
  auto u = new Unit;
  assert(inboard(p));
  assert(player_id >= 0 && player_id < 16);
  u->id = get_new_unit_id();
  u->pos = p;
  u->player_id = player_id;
  u->dir = static_cast<Dir>(rnd(0, 7));
  u->type_id = rnd(0, (int)Unit_type_id::UNIT_COUNT - 1);
  units.push_back(u);
  calculate_fow();
#if 0
  build_fow_array(&va_fog_of_war);
#endif
}

void Core::kill_unit(Unit* u) {
  // delete_node(&units, data2node(units, u));
  units.remove(u);
  delete u;
  if (selected_unit) {
    pathfinder.fill_map(*selected_unit);
    calculate_fow();
#if 0
    build_walkable_array(&va_walkable_map);
    build_fow_array(&va_fog_of_war);
#endif
  }
}

void Core::shoot(Unit *shooter, Unit *target) {
  if (isLosClear(shooter->pos, target->pos)) {
    kill_unit(target);
  }
}

void Core::init_units() {
  selected_unit = NULL;
  for (int i = 0; i < 8; i++) {
    V2i p = V2i(rnd(0, MAP_X - 1), rnd(0, MAP_Y - 1));
    if (!tile(p).obstacle && !unit_at(p)) {
      add_unit(p, rnd(0, 1));
    } else {
      i--;
    }
  }
}

void Core::init_obstacles() {
  V2i p;
  FOR_EACH_TILE(&p) {
    Tile& t = tile(p);
    t.obstacle = ((rand() % 100) > 85);
  }
}

void Core::init_players() {
  int id[] = {0, 1};
  init_local_players(2, id);
}

void Core::init_logic() {
  srand(time(NULL));
  init_unit_types();
  init_events();
  pathfinder.clean_map();
  clean_fow();
  init_players();
  init_obstacles();
  init_units();
  calculate_fow();
}
