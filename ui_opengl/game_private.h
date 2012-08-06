/* See LICENSE file for copyright and license details. */

/* User interface */

extern UnitMode unit_mode;
extern int last_move_index;
extern int current_move_index;
extern Va va_walkable_map;
extern Va va_fog_of_war;

void v2i_to_v2f(V2f *f, const V2i *i);
void draw_unit_circle(const Unit *u);
void draw_unit_model(const Unit *u);
void build_walkable_array(Va *v);
void build_fow_array(Va *v);
