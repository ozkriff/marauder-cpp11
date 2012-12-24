/* See LICENSE file for copyright and license details. */

/* Core */

#include <list>

extern std::list<Player*> players;
extern Event const *current_event;
extern Player *current_player;
extern Unit *selected_unit;
extern std::list<Unit*> units;

Unit* id2unit(int id);
void calculate_fow(void);
