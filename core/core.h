/* See LICENSE file for copyright and license details. */

#ifndef CORE_CORE_H
#define CORE_CORE_H

typedef struct {
  int type_id;
  int id;
  int player_id;
  Dir dir;
  int action_points;
  V2i pos;
} Unit;

typedef struct {
  int id;
  int last_event_id;
} Player;

typedef struct {
  Unit *unit;
  bool obstacle;
  /* TODO: move to pathfinding */
  int cost;
  Dir parent;
  Dir dir;
  int fow; /* how many units see this tile */
} Tile;

#define MAP_X 20
#define MAP_Y 20

void inc_v2i(V2i *pos);

Tile &tile(const V2i& p);
bool inboard(const V2i& p);
Unit* unit_at(const V2i& pos);

/* TODO */
void shoot(Unit *shooter, Unit *target);
void add_unit(V2i p, int player_id);
void init_logic(void);

#define FOR_EACH_TILE(p) \
  for (set_v2i(p, 0, 0); inboard(*p); inc_v2i(p))

/* Player.last_event_id */
#define HAVE_NOT_SEEN_ANY_EVENTS (-1)

#endif
