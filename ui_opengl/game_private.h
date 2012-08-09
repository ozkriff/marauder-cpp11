/* See LICENSE file for copyright and license details. */

/* User interface */

typedef enum {
  UI_MODE_NORMAL,
  UI_MODE_SHOW_EVENT
} UIMode;

extern UIMode ui_mode;
extern int last_move_index;
extern int current_move_index;
extern VertexArray va_walkable_map;
extern VertexArray va_fog_of_war;

void v2i_to_v2f(V2f *f, const V2i *i);
void draw_unit_circle(const Unit *u);
void draw_unit_model(const Unit *u);
void build_walkable_array(VertexArray *v);
void build_fow_array(VertexArray *v);
