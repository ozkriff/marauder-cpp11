/* See LICENSE file for copyright and license details. */

/* Core */

extern List players;
extern Event const *current_event;
extern Player *current_player;
extern Unit *selected_unit;
extern List units;

Unit* id2unit(int id);
void calculate_fow(void);
