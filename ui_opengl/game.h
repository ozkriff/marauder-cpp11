/* See LICENSE file for copyright and license details. */

#ifndef UI_OPENGL_GAME_H
#define UI_OPENGL_GAME_H

#include <SDL/SDL.h>

typedef enum {
  UI_MODE_NORMAL,
  UI_MODE_SHOW_EVENT
} UIMode;

extern UIMode ui_mode;
extern int last_move_index;
extern int current_move_index;
extern VertexArray va_walkable_map;
extern VertexArray va_fog_of_war;

class Game;

extern Game game;

class Game {
public:
  Game();
  ~Game();
  void do_xxx();
  void init();
  void cleanup();
  V2f v2i_to_v2f(const V2i& i);
  void build_map_array(VertexArray *v);
  void build_obstacles_array(VertexArray *v);
  int calculate_walkable_tiles_count();
  int calculate_fogged_tiles_count();
  void build_fow_array(VertexArray *v);
  void build_walkable_array(VertexArray *v);
  void draw_map();
  void draw_unit_model(const Unit *u);
  void draw_unit_circle(const Unit *u);
  void draw_unit(const Unit *u);
  void draw_units();
  void draw();
  void process_mouse_button_down_event(
      const SDL_MouseButtonEvent *e);
  void process_mouse_motion_event(
      const SDL_MouseMotionEvent *e);
  void process_key_down_event(
      const SDL_KeyboardEvent *e);
  void screen_scenario_main_events();
  void logic();
  void process_sdl_event(const SDL_Event *e);
  void sdl_events();
  void build_picking_tiles_array(VertexArray *va);
  bool pick_tile(V2i *p, const V2i *mouse_pos);
  void draw_for_picking();
  void scroll_map();
  void mainloop();
  void init_opengl();
  void init_camera();
  void init_vertex_arrays();
  void load_unit_resources();
  void on_test_button();
  void add_buttons();
};

#endif
