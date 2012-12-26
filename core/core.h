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

void inc_v2i(V2i *pos);

Tile &tile(const V2i& p);
bool inboard(const V2i& p);
Unit* unit_at(const V2i& pos);

/* TODO */
void shoot(Unit *shooter, Unit *target);
void add_unit(V2i p, int player_id);
void init_logic();

#define FOR_EACH_TILE(p) \
  for (*p = V2i(0, 0); inboard(*p); inc_v2i(p))

/* Player.last_event_id */
#define HAVE_NOT_SEEN_ANY_EVENTS (-1)

extern std::list<Player*> players;
extern Event const *current_event;
extern Player *current_player;
extern Unit *selected_unit;
extern std::list<Unit*> units;
extern Pathfinder pathfinder;

Unit* id2unit(int id);
void calculate_fow(void);

#endif
