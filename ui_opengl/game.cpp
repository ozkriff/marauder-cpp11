/* See LICENSE file for copyright and license details. */

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_ttf.h>
#include "core/v2i.h"
#include "core/math.h"
#include "core/dir.h"
#include "core/misc.h"
#include "core/unit_type.h"
#include "core/core.h"
#include "core/event/move.h"
#include "core/event/end_turn.h"
#include "core/event.h"
#include "core/core_private.h"
#include "core/path.h"
#include "core/los.h"
#include "ui_opengl/v3f.h"
#include "ui_opengl/v2f.h"
#include "ui_opengl/math.h"
#include "ui_opengl/vertex_array.h"
#include "ui_opengl/obj.h"
#include "ui_opengl/camera.h"
#include "ui_opengl/game.h"
#include "ui_opengl/gl.h"
#include "ui_opengl/widgets.h"
#include "ui_opengl/event/move.h"
#include "ui_opengl/event.h"
#include "ui_opengl/config.h"

#ifndef DATA_DIR
#error DATA_DIR undefined!
#endif
#define DATA(x) (DATA_DIR "/" x)

#define TILE_SIZE 6.0f
#define TILE_SIZE_2 (TILE_SIZE / 2.0f)

UIMode ui_mode;
int last_move_index;
int current_move_index;
VertexArray va_walkable_map;
VertexArray va_fog_of_war;

Game game;

static V2i win_size;
static V2i mouse_pos;
static V2i active_tile_pos;
static SDL_Surface *screen;
static bool is_rotating_camera;
static bool done;
static Camera camera;
static GLuint floor_texture;
static VertexArray va_map;
static VertexArray va_obstacles;
static VertexArray va_pick;
static ObjModel obj_units[UNIT_COUNT];
static VertexArray va_units[UNIT_COUNT];
static GLuint texture_units[UNIT_COUNT];
static TTF_Font *font;

Game::Game() {
}

Game::~Game() {
}

void Game::do_xxx() {
  init();
  mainloop();
  cleanup();
}

void Game::init() {
  init_logic();
  done = false;
  set_v2i(&win_size, WIN_WIDTH, WIN_HEIGHT);
  set_v2i(&active_tile_pos, 0, 0);
  set_v2i(&mouse_pos, 0, 0);
  ui_mode = UI_MODE_NORMAL;
  is_rotating_camera = false;
  SDL_Init(SDL_INIT_EVERYTHING);
  screen = SDL_SetVideoMode(win_size.x, win_size.y,
      32, SDL_OPENGL | SDL_GL_DOUBLEBUFFER);
  init_opengl();
  init_camera();
  init_widgets();
  font = open_font(DEFAULT_FONT, 10);
  add_buttons();
  load_texture(&floor_texture, DATA("floor.png"));
  load_unit_resources();
  init_vertex_arrays();
}

void Game::cleanup() {
  SDL_Quit();
}

V2f Game::v2i_to_v2f(const V2i& i) {
  assert(inboard(i));
  return V2f(i.x * TILE_SIZE, i.y * TILE_SIZE);
}

void Game::build_map_array(VertexArray *v) {
  V2i p;
  int i = 0; /* tile's index */
  v->count = MAP_X * MAP_Y * 6;
  if (v->v) {
    delete[] v->v;
    v->v = NULL;
  }
  if (v->t) {
    delete[] v->t;
    v->t = NULL;
  }
  v->v = (float*)new V3f[v->count];
  v->t = (float*)new V2f[v->count];
  FOR_EACH_TILE(&p) {
    Tile& t = tile(p);
    float n = TILE_SIZE_2;
    V2f pos;
    if (t.obstacle) {
      continue;
    }
    pos = v2i_to_v2f(p);
    set_xy(v->v, 3, i, 0, pos.x() - n, pos.y() - n);
    set_xy(v->v, 3, i, 1, pos.x() + n, pos.y() - n);
    set_xy(v->v, 3, i, 2, pos.x() + n, pos.y() + n);
    set_xy(v->t, 3, i, 0, 0, 0);
    set_xy(v->t, 3, i, 1, 1, 0);
    set_xy(v->t, 3, i, 2, 1, 1);
    i++;
    set_xy(v->v, 3, i, 0, pos.x() - n, pos.y() - n);
    set_xy(v->v, 3, i, 1, pos.x() + n, pos.y() + n);
    set_xy(v->v, 3, i, 2, pos.x() - n, pos.y() + n);
    set_xy(v->t, 3, i, 0, 0, 0);
    set_xy(v->t, 3, i, 1, 1, 1);
    set_xy(v->t, 3, i, 2, 0, 1);
    i++;
  }
}

void Game::build_obstacles_array(VertexArray *v) {
  V2i p;
  int i = 0; /* tile's index */
  v->count = MAP_X * MAP_Y * 6;
  if (v->v) {
    delete[] v->v;
    v->v = NULL;
  }
  if (v->t) {
    delete[] v->t;
    v->t = NULL;
  }
  v->v = (float *)new V3f[v->count];
  v->t = (float *)new V2f[v->count];
  FOR_EACH_TILE(&p) {
    Tile& t = tile(p);
    float n = TILE_SIZE_2;
    if (!t.obstacle) {
      continue;
    }
    V2f pos = v2i_to_v2f(p);
    set_xy(v->v, 3, i, 0, pos.x() - n, pos.y() - n);
    set_xy(v->v, 3, i, 1, pos.x() + n, pos.y() - n);
    set_xy(v->v, 3, i, 2, pos.x() + n, pos.y() + n);
    set_xy(v->t, 3, i, 0, 0, 0);
    set_xy(v->t, 3, i, 1, 1, 0);
    set_xy(v->t, 3, i, 2, 1, 1);
    i++;
    set_xy(v->v, 3, i, 0, pos.x() - n, pos.y() - n);
    set_xy(v->v, 3, i, 1, pos.x() + n, pos.y() + n);
    set_xy(v->v, 3, i, 2, pos.x() - n, pos.y() + n);
    set_xy(v->t, 3, i, 0, 0, 0);
    set_xy(v->t, 3, i, 1, 1, 1);
    set_xy(v->t, 3, i, 2, 0, 1);
    i++;
  }
}

int Game::calculate_walkable_tiles_count() {
  V2i p;
  int count = 0;
  FOR_EACH_TILE(&p) {
    Tile& t = tile(p);
    if (t.parent != D_NONE && t.cost != 30000) {
      count++;
    }
  }
  return count;
}

int Game::calculate_fogged_tiles_count() {
  int n = 0;
  V2i p;
  FOR_EACH_TILE(&p) {
    Tile& t = tile(p);
    if (t.fow == 0) {
      n++;
    }
  }
  return n;
}

void Game::build_fow_array(VertexArray *v) {
  V2i p;
  int i = 0; /* tile's index */
  v->count = calculate_fogged_tiles_count() * 6;
  if (v->v) {
    delete[] v->v;
    v->v = NULL;
  }
  v->v = (float *)new V3f[v->count];
  FOR_EACH_TILE(&p) {
    Tile& t = tile(p);
    float n = TILE_SIZE_2;
    if (t.fow > 0)
      continue;
    V2f pos = v2i_to_v2f(p);
    set_xyz(v->v, 3, i, 0, pos.x() - n, pos.y() - n, 0.05f);
    set_xyz(v->v, 3, i, 1, pos.x() + n, pos.y() - n, 0.05f);
    set_xyz(v->v, 3, i, 2, pos.x() + n, pos.y() + n, 0.05f);
    i++;
    set_xyz(v->v, 3, i, 0, pos.x() - n, pos.y() - n, 0.05f);
    set_xyz(v->v, 3, i, 1, pos.x() + n, pos.y() + n, 0.05f);
    set_xyz(v->v, 3, i, 2, pos.x() - n, pos.y() + n, 0.05f);
    i++;
  }
}

void Game::build_walkable_array(VertexArray *v) {
  V2i p;
  int i = 0; /* tile's index */
  assert(v);
  if (v->v) {
    delete[] v->v;
    v->v = NULL;
  }
  v->count = calculate_walkable_tiles_count() * 3;
  if (v->count == 0) {
    return;
  }
  v->v = (float *)new V3f[v->count];
  FOR_EACH_TILE(&p) {
    Tile& t = tile(p);
    if (t.parent != D_NONE && t.cost < 50) {
      V2i p2;
      neib(&p2, &p, t.parent);
      if (inboard(p2)) {
        V2f pos1 = v2i_to_v2f(p);
        V2f pos2 = v2i_to_v2f(p2);
        set_xyz(v->v, 2, i, 0, pos1.x(), pos1.y(), 0.1f);
        set_xyz(v->v, 2, i, 1, pos2.x(), pos2.y(), 0.1f);
        i++;
      }
    }
  }
}

void Game::draw_map() {
  glEnable(GL_TEXTURE_2D);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glBindTexture(GL_TEXTURE_2D, floor_texture);

  glColor3f(1.0f, 1.0f, 1.0f);
  glTexCoordPointer(2, GL_FLOAT, 0, va_map.t);
  glVertexPointer(2, GL_FLOAT, 0, va_map.v);
  glDrawArrays(GL_TRIANGLES, 0, va_map.count);

  glColor3f(1.0f, 0.0f, 0.0f);
  glTexCoordPointer(2, GL_FLOAT, 0, va_obstacles.t);
  glVertexPointer(2, GL_FLOAT, 0, va_obstacles.v);
  glDrawArrays(GL_TRIANGLES, 0, va_obstacles.count);

  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisable(GL_TEXTURE_2D);
  glDisableClientState(GL_VERTEX_ARRAY);

  glEnableClientState(GL_VERTEX_ARRAY);
  glColor3f(0.0f, 0.3f, 1.0f);
  glVertexPointer(3, GL_FLOAT, 0, va_walkable_map.v);
  glDrawArrays(GL_LINES, 0, va_walkable_map.count);
  glDisableClientState(GL_VERTEX_ARRAY);

  glEnableClientState(GL_VERTEX_ARRAY);
  glColor4f(0.0f, 0.0f, 0.0f, 0.2f);
  glVertexPointer(3, GL_FLOAT, 0, va_fog_of_war.v);
  glDrawArrays(GL_TRIANGLES, 0, va_fog_of_war.count);
  glDisableClientState(GL_VERTEX_ARRAY);
}

void Game::draw_unit_model(const Unit *u) {
  assert(u);
  glEnable(GL_TEXTURE_2D);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glBindTexture(GL_TEXTURE_2D, texture_units[u->type_id]);
  glColor3f(1, 1, 1);
  glTexCoordPointer(2, GL_FLOAT, 0, va_units[u->type_id].t);
  glVertexPointer(3, GL_FLOAT, 0, va_units[u->type_id].v);
  glDrawArrays(GL_TRIANGLES, 0, va_units[u->type_id].count);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisable(GL_TEXTURE_2D);
}

void Game::draw_unit_circle(const Unit *u) {
  float v[4 * 3];
  float n = TILE_SIZE_2 * 0.9f;
  assert(u);
  if (u->player_id == 0) {
    glColor3f(1, 0, 0);
  } else if (u->player_id == 1) {
    glColor3f(0, 0, 1);
  } else {
    die("draw_unit_circle(): You need more colors!");
  }
  set_xyz(v, 2, 0, 0, +n, +n, 0.1f);
  set_xyz(v, 2, 0, 1, +n, -n, 0.1f);
  set_xyz(v, 2, 1, 0, -n, -n, 0.1f);
  set_xyz(v, 2, 1, 1, -n, +n, 0.1f);
  glLineWidth(2);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, v);
  glDrawArrays(GL_LINE_LOOP, 0, 4);
  glDisableClientState(GL_VERTEX_ARRAY);
  glLineWidth(1);
}

void Game::draw_unit(const Unit *u) {
  assert(u);
  V2f f = v2i_to_v2f(u->pos);
  glColor3f(1, 0, 0);
  glPushMatrix();
  glTranslatef(f.x(), f.y(), 0);
  glRotatef((u->dir + 4) * 45.0f, 0, 0, 1);
  draw_unit_model(u);
  draw_unit_circle(u);
  glPopMatrix();
}

void Game::draw_units() {
  for (auto u : units) {
    if (ui_mode == UI_MODE_SHOW_EVENT
        && event_filter_unit(current_event, u))
    {
      continue;
    }
    if (tile(u->pos).fow == 0) {
      continue;
    }
    draw_unit(u);
  }
}

void Game::draw() {
  glLoadIdentity();
  camera.set();
  draw_map();
  draw_units();
  if (ui_mode == UI_MODE_SHOW_EVENT) {
    event_draw(current_event);
  }
  draw_buttons();
  SDL_GL_SwapBuffers();
}

void Game::process_mouse_button_down_event(
    const SDL_MouseButtonEvent *e)
{
  V2i p;
  Button *b;
  Unit *u;
  set_v2i(&p, static_cast<int>(e->x), static_cast<int>(e->y));
  b = v2i_to_button(p);
  if (b) {
    b->callback();
    return;
  }
  u = unit_at(active_tile_pos);
  Tile& t = tile(active_tile_pos);
  assert(current_player);
  assert(e);
  UNUSED(e);
  if (ui_mode != UI_MODE_NORMAL) {
    return;
  }
  if (u && u->player_id == current_player->id) {
    selected_unit = u;
    fill_map(selected_unit);
    build_walkable_array(&va_walkable_map);
  } else if (selected_unit) {
    const Unit_type *type = get_unit_type(
        selected_unit->type_id);
    int ap = type->action_points;
    if (u && u->player_id != current_player->id) {
      if (selected_unit && u) {
        shoot(selected_unit, u);
      }
    } else if (t.cost <= ap && t.parent != D_NONE) {
      generate_event_move(selected_unit, &active_tile_pos);
      /* TODO: Move this to ui_event_move? */
      delete[] va_walkable_map.v;
      va_walkable_map = empty_vertex_array;
    }
  }
}

void Game::process_mouse_motion_event(
    const SDL_MouseMotionEvent *e)
{
  assert(e);
  set_v2i(&mouse_pos, (int)e->x, (int)e->y);
  if (is_rotating_camera) {
    camera.z_angle -= e->xrel;
    camera.x_angle -= e->yrel;
    clamp_angle(&camera.z_angle);
    clamp_f(&camera.x_angle, 0, 50);
  }
}

void Game::process_key_down_event(
    const SDL_KeyboardEvent *e)
{
  assert(e);
  switch (e->keysym.sym) {
    case SDLK_ESCAPE:
    case SDLK_q: {
      done = true;
      break;
    }
    case SDLK_c: {
      if (!selected_unit) {
        return;
      }
      V2f unit_pos = v2i_to_v2f(selected_unit->pos);
      camera.pos = unit_pos;
      break;
    }
    case SDLK_t: {
      Tile& t = tile(active_tile_pos);
      t.obstacle = !t.obstacle;
      build_map_array(&va_map);
      build_obstacles_array(&va_obstacles);
      calculate_fow();
      build_fow_array(&va_fog_of_war);
      if (selected_unit) {
        fill_map(selected_unit);
        build_walkable_array(&va_walkable_map);
      }
      break;
    }
    case SDLK_e: {
      generate_event_end_turn();
      break;
    }
    case SDLK_u: {
      add_unit(active_tile_pos, current_player->id);
      if (selected_unit) {
        fill_map(selected_unit);
        build_walkable_array(&va_walkable_map);
      }
      break;
    }
    case SDLK_d: {
      camera.z_angle += 15;
      clamp_angle(&camera.z_angle);
      break;
    }
    case SDLK_a: {
      camera.z_angle -= 15;
      clamp_angle(&camera.z_angle);
      break;
    }
    case SDLK_w: {
      camera.zoom -= 10;
      clamp_f(&camera.zoom, 30, 200);
      break;
    }
    case SDLK_s: {
      camera.zoom += 10;
      clamp_f(&camera.zoom, 30, 200);
      break;
    }
    case SDLK_UP: {
      camera.move(D_N);
      break;
    }
    case SDLK_DOWN: {
      camera.move(D_S);
      break;
    }
    case SDLK_LEFT: {
      camera.move(D_W);
      break;
    }
    case SDLK_RIGHT: {
      camera.move(D_E);
      break;
    }
    default: {
      printf("process_key_down_event(): "
          "Unknown key (%d, 0x%x) was pressed.\n",
          e->keysym.sym, e->keysym.sym);
      break;
    }
  }
}

void Game::screen_scenario_main_events() {
  current_event = get_next_event();
  last_move_index = get_last_event_index(current_event);
  ui_mode = UI_MODE_SHOW_EVENT;
  current_move_index = 0;
  /* TODO: Remove this hack */
  if (current_event->t == E_END_TURN) {
    apply_event(current_event);
    ui_mode = UI_MODE_NORMAL;
  }
}

void Game::logic() {
  while (ui_mode == UI_MODE_NORMAL
      && unshown_events_left())
  {
    screen_scenario_main_events();
  }
}

void Game::process_sdl_event(const SDL_Event *e) {
  assert(e);
  switch (e->type) {
    case SDL_QUIT: {
      done = true;
      break;
    }
    case SDL_VIDEORESIZE: {
      screen = SDL_SetVideoMode(e->resize.w, e->resize.h,
          32, SDL_RESIZABLE);
      break;
    }
    case SDL_KEYDOWN: {
      process_key_down_event(&e->key);
      break;
    }
    case SDL_MOUSEMOTION: {
      process_mouse_motion_event(&e->motion);
      break;
    }
    case SDL_MOUSEBUTTONUP: {
      if (e->button.button == SDL_BUTTON_RIGHT) {
        is_rotating_camera = false;
      }
      if (e->button.button == SDL_BUTTON_WHEELUP) {
        camera.zoom -= 5;
        clamp_f(&camera.zoom, 30, 200);
      } else if (e->button.button == SDL_BUTTON_WHEELDOWN) {
        camera.zoom += 5;
        clamp_f(&camera.zoom, 30, 200);
      }
      break;
    }
    case SDL_MOUSEBUTTONDOWN: {
      if (e->button.button == SDL_BUTTON_RIGHT) {
        is_rotating_camera = true;
      } else if (e->button.button == SDL_BUTTON_LEFT) {
        process_mouse_button_down_event(&e->button);
      }
      break;
    }
    default: {
      break;
    }
  }
}

void Game::sdl_events() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    process_sdl_event(&e);
  }
}

void Game::build_picking_tiles_array(VertexArray *va) {
  V2i p;
  int i = 0; /* tile index */
  assert(va);
  va->count = MAP_X * MAP_Y * 4;
  if (va->v) {
    delete[] va->v;
    va->v = NULL;
  }
  if (va->ub_c) {
    delete[] va->ub_c;
    va->ub_c = NULL;
  }
  va->v = (float *)new V3f[va->count];
  va->ub_c = new GLubyte[va->count * 3];
  FOR_EACH_TILE(&p) {
    float n = TILE_SIZE_2;
    V2f pos = v2i_to_v2f(p);
    set_xy(va->v, 4, i, 0, pos.x() - n, pos.y() - n);
    set_xy(va->v, 4, i, 1, pos.x() + n, pos.y() - n);
    set_xy(va->v, 4, i, 2, pos.x() + n, pos.y() + n);
    set_xy(va->v, 4, i, 3, pos.x() - n, pos.y() + n);
    set_rgb_i(va->ub_c, 4, i, 0, p.x, p.y, 1);
    set_rgb_i(va->ub_c, 4, i, 1, p.x, p.y, 1);
    set_rgb_i(va->ub_c, 4, i, 2, p.x, p.y, 1);
    set_rgb_i(va->ub_c, 4, i, 3, p.x, p.y, 1);
    i++;
  }
}

bool Game::pick_tile(V2i *p, const V2i *mouse_pos) {
  GLint viewport[4];
  GLubyte pixel[3];
  assert(p);
  glGetIntegerv(GL_VIEWPORT, viewport);
  viewport[3] -= 1;
  glReadPixels(mouse_pos->x, viewport[3] - mouse_pos->y,
      1, 1, GL_RGB, GL_UNSIGNED_BYTE, (void *)pixel);
  if (pixel[2] != 1) {
    return false;
  }
  p->x = pixel[0];
  p->y = pixel[1];
  return true;
}

void Game::draw_for_picking() {
  glLoadIdentity();
  camera.set();
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glColorPointer(3, GL_UNSIGNED_BYTE, 0, va_pick.ub_c);
  glVertexPointer(2, GL_FLOAT, 0, va_pick.v);
  glDrawArrays(GL_QUADS, 0, va_pick.count);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}

void Game::scroll_map() {
  const V2i *p = &mouse_pos;
  int offset = 15;
  if (p->x < offset) {
    camera.move(D_W);
  } else if(p->x > screen->w - offset) {
    camera.move(D_E);
  }
  if (p->y < offset) {
    camera.move(D_N);
  } else if(p->y > screen->h - offset) {
    camera.move(D_S);
  }
  if (camera.pos.x() > MAP_X * TILE_SIZE) {
    camera.pos.setX(MAP_X * TILE_SIZE);
  } else if (camera.pos.x() < 0) {
    camera.pos.setX(0);
  }
  if (camera.pos.y() > MAP_Y * TILE_SIZE) {
    camera.pos.setY(MAP_Y * TILE_SIZE);
  } else if (camera.pos.y() < 0) {
    camera.pos.setY(0);
  }
}

void Game::mainloop() {
  while (!done) {
    sdl_events();
    logic();
    scroll_map();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw_for_picking();
    pick_tile(&active_tile_pos, &mouse_pos);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw();
    // SDL_Delay(1000.0f / 24.0f);
  }
}

void Game::init_opengl() {
  float aspect_ratio = (float)win_size.y / win_size.x;
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(0.5, -0.5, -0.5 * aspect_ratio,
      0.5 * aspect_ratio, 1, 500);
  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_BLEND);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_COLOR_MATERIAL);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glClearColor(0, 0, 0, 1);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0.0f);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Game::init_camera() {
  camera.x_angle = 45.0f;
  camera.z_angle = 45.0f;
  camera.pos = V2f(20, 20);
  camera.zoom = 100.0f;
}

void Game::init_vertex_arrays() {
  va_map = empty_vertex_array;
  va_pick = empty_vertex_array;
  va_walkable_map = empty_vertex_array;
  va_fog_of_war = empty_vertex_array;
  build_picking_tiles_array(&va_pick);
  build_map_array(&va_map);
  build_obstacles_array(&va_obstacles);
  build_fow_array(&va_fog_of_war);
}

void Game::load_unit_resources() {
  load_texture(&texture_units[UNIT_TANK], DATA("tank.png"));
  load_texture(&texture_units[UNIT_TRUCK], DATA("truck.png"));
  obj_read(&obj_units[UNIT_TANK], DATA("tank.obj"));
  obj_read(&obj_units[UNIT_TRUCK], DATA("truck.obj"));
  obj_build(&va_units[UNIT_TANK], &obj_units[UNIT_TANK]);
  obj_build(&va_units[UNIT_TRUCK], &obj_units[UNIT_TRUCK]);
}

void Game::on_test_button() {
  printf("BUTTON CLICKED\n");
}

void Game::add_buttons() {
  V2i pos = {10, 10};
  (void)add_button(font, &pos, "[CLICK ME!]",
      nullptr); // on_test_button);
}

int main(int ac, char **av) {
  UNUSED(ac);
  UNUSED(av);
  Game game;
  game.do_xxx();
  return 0;
}
