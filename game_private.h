/* See LICENSE file for copyright and license details. */

/* Core */

/* TODO: Rename */
typedef enum {
  UM_NORMAL,
  UM_MOVING
} UnitMode;

extern List players;
extern Event const *current_event;
extern Player *current_player;
extern Unit *selected_unit;

Unit* id2unit(int id);
void calculate_fow(void);
