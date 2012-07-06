/* See LICENSE file for copyright and license details. */

typedef struct {
  int type_id;
  int id;
  Dir dir;
  int action_points;
  V2i pos;
} Unit;

typedef struct {
  Unit *unit_id;
  bool is_path;
  bool obstacle;
  /* TODO: move to pathfinding */
  int cost;
  Dir parent;
  Dir dir;
} Tile;

#define MAP_X 50
#define MAP_Y 50

void inc_v2i(V2i *pos);

Tile* tile(const V2i *p);
bool inboard(const V2i *p);

extern int action_points;
