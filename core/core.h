/* See LICENSE file for copyright and license details. */

#ifndef CORE_CORE_H
#define CORE_CORE_H

#include <list>
#include "core/misc.h"
#include "core/v2i.h"
#include "core/math.h"
#include "core/dir.h"
#include "core/los.h"
#include "core/unit_type.h"
#include "core/event.h"
#include "core/path.h"

struct Unit {
  int type_id;
  int id;
  int player_id;
  Dir dir;
  int action_points;
  V2i pos;
};

struct Player {
  int id;
  int last_event_id;
};

struct Tile {
  Unit *unit;
  bool obstacle;
  /* TODO: move to pathfinding */
  int cost;
  Dir parent;
  Dir dir;
  int fow; /* how many units see this tile */
};

#define MAP_X 20
#define MAP_Y 20

// void inc_v2i(V2i *pos);

/* TODO */
// void shoot(Unit *shooter, Unit *target);
// void add_unit(V2i p, int player_id);
// void init_logic();

/* Player.last_event_id */
#define HAVE_NOT_SEEN_ANY_EVENTS (-1)

class Core {
public:
  std::list<Player*> players;
  Event const* current_event;
  Player* current_player;
  Unit* selected_unit;
  std::list<Unit*> units;
  Pathfinder pathfinder;
  Tile map[MAP_Y][MAP_X];
  std::list<Event*> events;

  Core();
  ~Core();

  Tile& tile(const V2i &p);
  void calculate_fow();
  Unit* unit_at(const V2i& pos);
  Unit* id2unit(int id);
  void add_unit(const V2i& p, int player_id);
  void shoot(Unit *shooter, Unit *target);
  void kill_unit(Unit* u);
  void init_logic();
  bool inboard(const V2i& p) const;
  void inc_v2i(V2i* pos) const;

private:
  void create_local_human(int id);
  void init_local_players(int n, int* ids);
  int get_new_unit_id();
  void init_units();
  void init_players();
  void init_obstacles();
  void clean_fow();
  bool isLosClear(const V2i& from, const V2i& to);
};

// Unit* id2unit(int id);
// void calculate_fow(void);

#endif
