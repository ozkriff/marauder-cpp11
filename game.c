/* See LICENSE file for copyright and license details. */

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#include "bool.h"
#include "list.h"
#include "v2i.h"
#include "v3f.h"
#include "v2f.h"
#include "math.h"
#include "dir.h"
#include "line2f.h"
#include "va.h"
#include "obj.h"
#include "misc.h"
#include "camera.h"
#include "game.h"
#include "path.h"
#include "gl.h"
#include "los.h"

#define DATA(x) "../data/" x

typedef enum {
  UM_NORMAL,
  UM_MOVING
} Unit_mode;

typedef struct {
  int id;
} Player;

static const Va empty_va = {NULL, NULL, NULL, 0};
static const List empty_list = {NULL, NULL, 0};

#define TILE_SIZE 6.0f
#define TILE_SIZE_2 (TILE_SIZE / 2.0f)

static Tile map[MAP_Y][MAP_X];

int action_points;

static Unit_mode unit_mode;
static int last_move_index;
static int current_move_index;
static List move_path;
static int move_speed;
static V2i win_size;
static V2i mouse_pos;
static V2i active_tile_pos;
static SDL_Surface *screen;
static bool is_dragging_map;
static bool done;
static Camera camera;
static GLuint floor_texture;
static GLuint unit_texture;
static Obj_model test_obj_model;
static Va va_map;
static Va va_obstacles;
static Va va_walkable_map;
static Va va_obj;
static Va va_pick;
static Va va_fow; /* fog of war */
static List units;
static Unit *selected_unit;

static Player *current_player;
static List players;

void create_local_human(int id) {
  Player *p = ALLOCATE(1, Player);
  Node *n = ALLOCATE(1, Node);
  set_node(n, p);
  push_node(&players, n);
  p->id = id;
}

void init_local_players(int n, int *ids) {
  int i;
  for (i = 0; i < n; i++) {
    create_local_human(ids[i]);
  }
  current_player = players.tail->data;
}

bool inboard(const V2i *p) {
  assert(p);
  return p->x >= 0 && p->y >= 0
      && p->x < MAP_X && p->y < MAP_Y;
}

Tile* tile(const V2i *p) {
  assert(p);
  assert(inboard(p));
  return &(map[p->y][p->x]);
}

/* TODO rename */
void inc_v2i(V2i *pos) {
  assert(pos);
  assert(inboard(pos));
  pos->x++;
  if (pos->x == MAP_X) {
    pos->x = 0;
    pos->y++;
  }
}

static void v2i_to_v2f(V2f *f, const V2i *i) {
  assert(f);
  assert(i);
  assert(inboard(i));
  f->x = i->x * TILE_SIZE;
  f->y = i->y * TILE_SIZE;
}

static void build_map_array(Va *v) {
  V2i p;
  int i = 0; /* tile's index */
  v->count = MAP_X * MAP_Y * 6;
  if (v->v) {
    free(v->v);
    v->v = NULL;
  }
  if (v->t) {
    free(v->t);
    v->t = NULL;
  }
  v->v = ALLOCATE(v->count, V3f);
  v->t = ALLOCATE(v->count, V2f);
  for (set_v2i(&p, 0, 0); inboard(&p); inc_v2i(&p)) {
    Tile *t = tile(&p);
    float n = TILE_SIZE_2;
    V2f pos;
    if (t->obstacle) {
      continue;
    }
    v2i_to_v2f(&pos, &p);
    assert(t);
    set_xy(v->v, 3, i, 0, pos.x - n, pos.y - n);
    set_xy(v->v, 3, i, 1, pos.x + n, pos.y - n);
    set_xy(v->v, 3, i, 2, pos.x + n, pos.y + n);
    set_xy(v->t, 3, i, 0, 0, 0);
    set_xy(v->t, 3, i, 1, 1, 0);
    set_xy(v->t, 3, i, 2, 1, 1);
    i++;
    set_xy(v->v, 3, i, 0, pos.x - n, pos.y - n);
    set_xy(v->v, 3, i, 1, pos.x + n, pos.y + n);
    set_xy(v->v, 3, i, 2, pos.x - n, pos.y + n);
    set_xy(v->t, 3, i, 0, 0, 0);
    set_xy(v->t, 3, i, 1, 1, 1);
    set_xy(v->t, 3, i, 2, 0, 1);
    i++;
  }
}

static void build_obstacles_array(Va *v) {
  V2i p;
  int i = 0; /* tile's index */
  v->count = MAP_X * MAP_Y * 6;
  if (v->v) {
    free(v->v);
    v->v = NULL;
  }
  if (v->t) {
    free(v->t);
    v->t = NULL;
  }
  v->v = ALLOCATE(v->count, V3f);
  v->t = ALLOCATE(v->count, V2f);
  for (set_v2i(&p, 0, 0); inboard(&p); inc_v2i(&p)) {
    Tile *t = tile(&p);
    float n = TILE_SIZE_2;
    V2f pos;
    if (!t->obstacle) {
      continue;
    }
    v2i_to_v2f(&pos, &p);
    assert(t);
    set_xy(v->v, 3, i, 0, pos.x - n, pos.y - n);
    set_xy(v->v, 3, i, 1, pos.x + n, pos.y - n);
    set_xy(v->v, 3, i, 2, pos.x + n, pos.y + n);
    set_xy(v->t, 3, i, 0, 0, 0);
    set_xy(v->t, 3, i, 1, 1, 0);
    set_xy(v->t, 3, i, 2, 1, 1);
    i++;
    set_xy(v->v, 3, i, 0, pos.x - n, pos.y - n);
    set_xy(v->v, 3, i, 1, pos.x + n, pos.y + n);
    set_xy(v->v, 3, i, 2, pos.x - n, pos.y + n);
    set_xy(v->t, 3, i, 0, 0, 0);
    set_xy(v->t, 3, i, 1, 1, 1);
    set_xy(v->t, 3, i, 2, 0, 1);
    i++;
  }
}

static int calculate_walkable_tiles_count(void) {
  V2i p;
  int count = 0;
  for (set_v2i(&p, 0, 0); inboard(&p); inc_v2i(&p)) {
    Tile *t = tile(&p);
    if (t->parent != D_NONE && t->cost != 30000) {
      count++;
    }
  }
  return count;
}

static bool is_los_clear(const V2i *p1, const V2i *p2) {
  LosData los_data;
  V2i p = {0, 0};
  los_init(&los_data, p1, p2);
  do {
    /* TODO variable p has wrong value here! */
    if (unit_at(&p) || tile(&p)->obstacle)
      return false;
    los_get_next(&los_data, &p);
  } while (!los_is_finished(&los_data));
  return true;
}

static void calculate_fow(void) {
  V2i p;
  assert(current_player);
  for (set_v2i(&p, 0, 0); inboard(&p); inc_v2i(&p)) {
    Tile *t = tile(&p);
    Node *node;
    t->fow = 0;
    FOR_EACH_NODE(units, node) {
      Unit *u = node->data;
      bool is_player_ok = (u->player_id == current_player->id);
      bool is_distance_ok = is_los_clear(&p, &u->pos);
      bool is_los_ok = (dist_i(&p, &u->pos) < 9);
      if (is_player_ok && is_distance_ok && is_los_ok) {
        t->fow++;
      }
    }
  }
}

static int calculate_fogged_tiles_count(void) {
  int n = 0;
  V2i p;
  for (set_v2i(&p, 0, 0); inboard(&p); inc_v2i(&p)) {
    Tile *t = tile(&p);
    if (t->fow == 0) {
      n++;
    }
  }
  return n;
}

static void build_fow_array(Va *v) {
  V2i p;
  int i = 0; /* tile's index */
  v->count = calculate_fogged_tiles_count() * 6;
  if (v->v) {
    free(v->v);
    v->v = NULL;
  }
  v->v = ALLOCATE(v->count, V3f);
  for (set_v2i(&p, 0, 0); inboard(&p); inc_v2i(&p)) {
    Tile *t = tile(&p);
    float n = TILE_SIZE_2;
    V2f pos;
    if (t->fow > 0)
      continue;
    v2i_to_v2f(&pos, &p);
    assert(t);
    set_xyz(v->v, 3, i, 0, pos.x - n, pos.y - n, 0.05f);
    set_xyz(v->v, 3, i, 1, pos.x + n, pos.y - n, 0.05f);
    set_xyz(v->v, 3, i, 2, pos.x + n, pos.y + n, 0.05f);
    i++;
    set_xyz(v->v, 3, i, 0, pos.x - n, pos.y - n, 0.05f);
    set_xyz(v->v, 3, i, 1, pos.x + n, pos.y + n, 0.05f);
    set_xyz(v->v, 3, i, 2, pos.x - n, pos.y + n, 0.05f);
    i++;
  }
}

static void build_walkable_array(Va *v) {
  V2i p;
  int i = 0; /* tile's index */
  assert(v);
  if (v->v) {
    free(v->v);
    v->v = NULL;
  }
  v->count = calculate_walkable_tiles_count() * 3;
  if (v->count == 0) {
    return;
  }
  v->v = ALLOCATE(v->count, V3f);
  for (set_v2i(&p, 0, 0); inboard(&p); inc_v2i(&p)) {
    Tile *t = tile(&p);
    assert(t);
    if (t->parent != D_NONE && t->cost < 50) {
      V2f pos1, pos2;
      V2i p2;
      neib(&p2, &p, t->parent);
      if (inboard(&p2)) {
        v2i_to_v2f(&pos1, &p);
        v2i_to_v2f(&pos2, &p2);
        set_xyz(v->v, 2, i, 0, pos1.x, pos1.y, 0.1f);
        set_xyz(v->v, 2, i, 1, pos2.x, pos2.y, 0.1f);
        i++;
      }
    }
  }
}

Unit* unit_at(const V2i *pos) {
  Node *node;
  assert(pos);
  FOR_EACH_NODE(units, node) {
    Unit *u = node->data;
    if (v2i_is_equal(&u->pos, pos)) {
      return u;
    }
  }
  return NULL;
}

static void add_unit(V2i p, int player_id) {
  Unit *u = ALLOCATE(1, Unit);
  Node *n = ALLOCATE(1, Node);
  assert(inboard(&p));
  assert(player_id >= 0 && player_id < 16);
  set_node(n, u);
  push_node(&units, n);
  u->pos = p;
  u->player_id = player_id;
  u->dir = (Dir)rnd(0, 7);
  calculate_fow();
  build_fow_array(&va_fow);
}

static void draw_map(void) {
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
  glVertexPointer(3, GL_FLOAT, 0, va_fow.v);
  glDrawArrays(GL_TRIANGLES, 0, va_fow.count);
  glDisableClientState(GL_VERTEX_ARRAY);
}

static void draw_unit_model(void) {
  glEnable(GL_TEXTURE_2D);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glBindTexture(GL_TEXTURE_2D, unit_texture);
  glColor3f(1, 1, 1);
  glTexCoordPointer(2, GL_FLOAT, 0, va_obj.t);
  glVertexPointer(3, GL_FLOAT, 0, va_obj.v);
  glDrawArrays(GL_TRIANGLES, 0, va_obj.count);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisable(GL_TEXTURE_2D);
}

static void draw_unit_circle(const Unit *u) {
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

static void draw_units_at_f(const V2f *p, int n) {
  int i;
  assert(p);
  assert(n > 1);
  glColor3f(1, 0, 0);
  glPushMatrix();
  glTranslatef(p->x, p->y, 0);
  /* glRotatef(90, 1, 0, 0); */
  glScalef(0.5f, 0.5f, 0.5f);
  for (i = 0; i < n; i++) {
    glPushMatrix();
    glRotatef(i * 360.0f / (float)n, 0, 0, 1);
    glTranslatef(TILE_SIZE_2 * 1.6f, TILE_SIZE_2 * 1.6f, 0);
    draw_unit_model();
    glPopMatrix();
  }
  glPopMatrix();
}

static void draw_unit(const Unit *u) {
  V2f f;
  assert(u);
  v2i_to_v2f(&f, &u->pos);
  glColor3f(1, 0, 0);
  glPushMatrix();
  glTranslatef(f.x, f.y, 0);
  glRotatef((u->dir + 4) * 45.0f, 0, 0, 1);
  draw_unit_model();
  draw_unit_circle(u);
  glPopMatrix();
}

static void draw_units(void) {
  Node *node;
  FOR_EACH_NODE(units, node) {
    Unit *u = node->data;
    if (unit_mode == UM_MOVING && u == selected_unit) {
      continue;
    }
    draw_unit(u);
  }
}

static void get_current_moving_nodes(V2i *from, V2i *to) {
  Node *node;
  int i = current_move_index;
  assert(from);
  assert(to);
  FOR_EACH_NODE(move_path, node) {
    i -= move_speed;
    if (i < 0) {
      break;
    }
  }
  *from = *(V2i*)node->data;
  *to = *(V2i*)node->next->data;
}

static void end_movement(const V2i *pos) {
  assert(pos);
  while (move_path.count > 0) {
    delete_node(&move_path, move_path.head);
  }
  unit_mode = UM_NORMAL;
  selected_unit->pos = *pos;
  fill_map(selected_unit);
  build_walkable_array(&va_walkable_map);
  calculate_fow();
  build_fow_array(&va_fow);
}

static void draw_moving_unit(void) {
  V2i from_i, to_i;
  V2f from_f, to_f;
  int node_index;
  V2f diff;
  V2f p;
  get_current_moving_nodes(&from_i, &to_i);
  v2i_to_v2f(&from_f, &from_i);
  v2i_to_v2f(&to_f, &to_i);
  node_index = current_move_index % move_speed;
  diff.x = (to_f.x - from_f.x) / move_speed;
  diff.y = (to_f.y - from_f.y) / move_speed;
  p.x = from_f.x + diff.x * node_index;
  p.y = from_f.y + diff.y * node_index;
  glPushMatrix();
  glTranslatef(p.x, p.y, 0.0f);
  glRotatef((m2dir(&from_i, &to_i) + 4) * 45.0f, 0, 0, 1);
  glEnable(GL_TEXTURE_2D);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  {
    glColor3f(1, 1, 1);
    glBindTexture(GL_TEXTURE_2D, unit_texture);
    glTexCoordPointer(2, GL_FLOAT, 0, va_obj.t);
    glVertexPointer(3, GL_FLOAT, 0, va_obj.v);
    glDrawArrays(GL_TRIANGLES, 0, va_obj.count);
  }
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisable(GL_TEXTURE_2D);
  glDisableClientState(GL_VERTEX_ARRAY);
  draw_unit_circle(selected_unit);
  glPopMatrix();
  current_move_index++;
  if (current_move_index == last_move_index) {
    selected_unit->dir = m2dir(&from_i, &to_i);
    end_movement(&to_i);
  }
}

static void draw(void) {
  glLoadIdentity();
  set_camera(&camera);
  draw_map();
  draw_units();
  if (unit_mode == UM_MOVING) {
    draw_moving_unit();
  }
  SDL_GL_SwapBuffers();
}

static void kill_unit(Unit *u) {
  delete_node(&units, data2node(units, u));
  if (selected_unit) {
    fill_map(selected_unit);
    build_walkable_array(&va_walkable_map);
    calculate_fow();
    build_fow_array(&va_fow);
  }
}

static void shoot(void) {
  LosData los_data;
  V2i p = {0, 0};
  Unit *u = unit_at(&active_tile_pos);
  if (!selected_unit || !u)
    return;
  /* calculate line of sight */
  los_init(&los_data, &selected_unit->pos,
      &active_tile_pos);
  los_get_next(&los_data, &p); /* Skip shooting unit. */
  do {
    if (unit_at(&p) || tile(&p)->obstacle)
      return;
    los_get_next(&los_data, &p);
  } while (!los_is_finished(&los_data));
  kill_unit(u);
}

static void process_mouse_button_down_event(
    const SDL_MouseButtonEvent *e)
{
  V2i v;
  assert(e);
  v.x = e->x / TILE_SIZE;
  v.y = e->y / TILE_SIZE;
  UNUSED(v);
  {
    int max_cost = 25;
    Unit *u = unit_at(&active_tile_pos);
    Tile *t = tile(&active_tile_pos);
    assert(current_player);
    if (unit_mode != UM_NORMAL) {
      return;
    }
    if (u && u->player_id == current_player->id) {
      selected_unit = u;
      fill_map(selected_unit);
      build_walkable_array(&va_walkable_map);
    } else if (selected_unit) {
      if (u && u->player_id != current_player->id) {
        shoot();
      } else if (t->cost < max_cost && t->parent != D_NONE) {
        fill_map(selected_unit);
        move_path = get_path(active_tile_pos);
        unit_mode = UM_MOVING;
        current_move_index = 0;
        last_move_index = (move_path.count - 1) * move_speed;
        free(va_walkable_map.v);
        va_walkable_map = empty_va;
      }
    }
  }
}

static void process_mouse_motion_event(
    const SDL_MouseMotionEvent *e)
{
  assert(e);
  set_v2i(&mouse_pos, (int)e->x, (int)e->y);
}

static void process_key_down_event(
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
      V2f unit_pos;
      if (!selected_unit)
        return;
      v2i_to_v2f(&unit_pos, &selected_unit->pos);
      set_v2f(&camera.pos, -unit_pos.x, -unit_pos.y);
      break;
    }
    case SDLK_t: {
      Tile *t = tile(&active_tile_pos);
      t->obstacle = !t->obstacle;
      build_map_array(&va_map);
      build_obstacles_array(&va_obstacles);
      if (selected_unit) {
        fill_map(selected_unit);
        build_walkable_array(&va_walkable_map);
        calculate_fow();
        build_fow_array(&va_fow);
      }
      break;
    }
    case SDLK_e: {
      /* end turn */
      Node *n = data2node(players, current_player)->next;
      if (n)
        current_player = n->data;
      else
        current_player = players.head->data;
      selected_unit = NULL;
      clean_map();
      free(va_walkable_map.v);
      va_walkable_map = empty_va;
      calculate_fow();
      build_fow_array(&va_fow);
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
      fix_angle(&camera.z_angle);
      break;
    }
    case SDLK_a: {
      camera.z_angle -= 15;
      fix_angle(&camera.z_angle);
      break;
    }
    case SDLK_w: {
      camera.zoom -= 10;
      break;
    }
    case SDLK_s: {
      camera.zoom += 10;
      break;
    }
    case SDLK_UP: {
      move_camera(&camera, D_UP);
      break;
    }
    case SDLK_DOWN: {
      move_camera(&camera, D_DOWN);
      break;
    }
    case SDLK_LEFT: {
      move_camera(&camera, D_LEFT);
      break;
    }
    case SDLK_RIGHT: {
      move_camera(&camera, D_RIGHT);
      break;
    }
    case SDLK_EQUALS: {
      action_points += 1;
      if (selected_unit) {
        fill_map(selected_unit);
        build_walkable_array(&va_walkable_map);
      }
      break;
    }
    case SDLK_MINUS: {
      action_points -= 1;
      if (selected_unit) {
        fill_map(selected_unit);
        build_walkable_array(&va_walkable_map);
      }
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

static void process_sdl_event(const SDL_Event *e) {
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
    case SDL_MOUSEBUTTONDOWN: {
      process_mouse_button_down_event(&e->button);
      break;
    }
    default: {
      break;
    }
  }
}

static void sdl_events(void) {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    process_sdl_event(&e);
  }
}

static void build_picking_tiles_array(Va *va) {
  V2i p;
  int i = 0; /* tile index */
  assert(va);
  va->count = MAP_X * MAP_Y * 4;
  if (va->v) {
    free(va->v);
    va->v = NULL;
  }
  if (va->ub_c) {
    free(va->ub_c);
    va->ub_c = NULL;
  }
  va->v = ALLOCATE(va->count, V3f);
  va->ub_c = ALLOCATE(va->count * 3, GLubyte);
  for (set_v2i(&p, 0, 0); inboard(&p); inc_v2i(&p)) {
    float n = TILE_SIZE_2;
    V2f pos;
    v2i_to_v2f(&pos, &p);
    set_xy(va->v, 4, i, 0, pos.x - n, pos.y - n);
    set_xy(va->v, 4, i, 1, pos.x + n, pos.y - n);
    set_xy(va->v, 4, i, 2, pos.x + n, pos.y + n);
    set_xy(va->v, 4, i, 3, pos.x - n, pos.y + n);
    set_rgb_i(va->ub_c, 4, i, 0, p.x, p.y, 1);
    set_rgb_i(va->ub_c, 4, i, 1, p.x, p.y, 1);
    set_rgb_i(va->ub_c, 4, i, 2, p.x, p.y, 1);
    set_rgb_i(va->ub_c, 4, i, 3, p.x, p.y, 1);
    i++;
  }
}

static bool pick_tile(V2i *p, const V2i *mouse_pos) {
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

static void draw_for_picking(void) {
  glLoadIdentity();
  set_camera(&camera);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glColorPointer(3, GL_UNSIGNED_BYTE, 0, va_pick.ub_c);
  glVertexPointer(2, GL_FLOAT, 0, va_pick.v);
  glDrawArrays(GL_QUADS, 0, va_pick.count);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}

static void mainloop(void) {
  while (!done) {
    sdl_events();
    /* logic(); */
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw_for_picking();
    pick_tile(&active_tile_pos, &mouse_pos);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw();
    SDL_Delay(1000.0f / 24.0f);
  }
}

static void init_opengl(void) {
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

static void init_camera(void){
  camera.x_angle = 45.0f;
  camera.z_angle = 45.0f;
  camera.pos.x = -20;
  camera.pos.y = -20;
  camera.zoom = 100.0f;
}

static void init_units(void) {
  int i;
  units = empty_list;
  selected_unit = NULL;
  for (i = 0; i < 20; i++) {
    V2i p;
    set_v2i(&p, rnd(0, MAP_X - 1), rnd(0, MAP_Y - 1));
    if (!tile(&p)->obstacle && !unit_at(&p)) {
      add_unit(p, rnd(0, 1));
    } else {
      i--;
    }
  }
}

static void init_obstacles(void) {
  V2i p;
  for (set_v2i(&p, 0, 0); inboard(&p); inc_v2i(&p)) {
    Tile *t = tile(&p);
    t->obstacle = ((rand() % 100) > 85);
  }
}

static void init_players(void) {
  int id[] = {0, 1};
  init_local_players(2, id);
}

static void init_logic(void) {
  srand(time(NULL));
  action_points = 24;
  init_pathfinding_module();
  clean_map();
  move_path = empty_list;
  players = empty_list;
  init_players();
  init_obstacles();
  init_units();
  calculate_fow();
}

/* TODO */
static void init_ui_opengl(void) {
  init_logic();
  done = false;
  set_v2i(&win_size, 400, 200);
  set_v2i(&active_tile_pos, 0, 0);
  set_v2i(&mouse_pos, 0, 0);
  SDL_Init(SDL_INIT_EVERYTHING);
  screen = SDL_SetVideoMode(win_size.x, win_size.y,
      32, SDL_OPENGL | SDL_GL_DOUBLEBUFFER);
  init_opengl();
  is_dragging_map = false;
  init_camera();
  obj_read(&test_obj_model, DATA("tank.obj"));
  obj_build(&va_obj, &test_obj_model);
  load_texture(DATA("floor.png"), &floor_texture);
  load_texture(DATA("tank.png"), &unit_texture);
  va_map = empty_va;
  va_pick = empty_va;
  build_picking_tiles_array(&va_pick);
  va_walkable_map = empty_va;
  unit_mode = UM_NORMAL;
  move_speed = 10;
  build_map_array(&va_map);
  build_obstacles_array(&va_obstacles);
  va_fow = empty_va;
  build_fow_array(&va_fow);
}

static void cleanup_opengl_ui(void) {
  SDL_Quit();
}

static void do_ui_opengl(void) {
  init_ui_opengl();
  mainloop();
  cleanup_opengl_ui();
}

int main(int ac, char **av) {
  UNUSED(ac);
  UNUSED(av);
  UNUSED(draw_units_at_f);
  do_ui_opengl();
  return 0;
}
