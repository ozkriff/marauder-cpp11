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
#include "path.h"
#include "game.h"
#include "gl.h"

#define DATA(x) "../data/" x

typedef enum {
  UM_NORMAL,
  UM_MOVING
} Unit_mode;

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
static Va va_walkable_map;
static Va va_obj;
static Va va_pick;
static List units;
static Unit *selected_unit;

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

static void build_map_array(void) {
  V2i p;
  int i = 0; /* tile's index */
  va_map.count = MAP_X * MAP_Y * 6;
  if (va_map.v) {
    free(va_map.v);
    va_map.v = NULL;
  }
  if (va_map.t) {
    free(va_map.t);
    va_map.t = NULL;
  }
  va_map.v = ALLOCATE(va_map.count, V3f);
  va_map.t = ALLOCATE(va_map.count, V2f);
  for (set_v2i(&p, 0, 0); inboard(&p); inc_v2i(&p)) {
    Tile *t = tile(&p);
    float n = TILE_SIZE_2;
    V2f pos;
    if (t->obstacle) {
      continue;
    }
    v2i_to_v2f(&pos, &p);
    assert(t);
    set_xy(va_map.v, 3, i, 0, pos.x - n, pos.y - n);
    set_xy(va_map.v, 3, i, 1, pos.x + n, pos.y - n);
    set_xy(va_map.v, 3, i, 2, pos.x + n, pos.y + n);
    set_xy(va_map.t, 3, i, 0, 0, 0);
    set_xy(va_map.t, 3, i, 1, 1, 0);
    set_xy(va_map.t, 3, i, 2, 1, 1);
    i++;
    set_xy(va_map.v, 3, i, 0, pos.x - n, pos.y - n);
    set_xy(va_map.v, 3, i, 1, pos.x + n, pos.y + n);
    set_xy(va_map.v, 3, i, 2, pos.x - n, pos.y + n);
    set_xy(va_map.t, 3, i, 0, 0, 0);
    set_xy(va_map.t, 3, i, 1, 1, 1);
    set_xy(va_map.t, 3, i, 2, 0, 1);
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

static void add_unit(V2i p) {
  Unit *u = ALLOCATE(1, Unit);
  Node *n = ALLOCATE(1, Node);
  set_node(n, u);
  push_node(&units, n);
  u->pos = p;
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

  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisable(GL_TEXTURE_2D);
  glDisableClientState(GL_VERTEX_ARRAY);

  glEnableClientState(GL_VERTEX_ARRAY);
  glColor3f(1.0f, 0.0f, 0.0f);
  glVertexPointer(3, GL_FLOAT, 0, va_walkable_map.v);
  glDrawArrays(GL_LINES, 0, va_walkable_map.count);
  glDisableClientState(GL_VERTEX_ARRAY);
}

static void draw_line(const Line2f *line) {
  assert(line);
  glBegin(GL_LINES);
  glVertex2f(line->a.x, line->a.y);
  glVertex2f(line->b.x, line->b.y);
  glEnd();
}

static void draw_unit(void) {
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

static void draw_unit_at_f(const V2f *p) {
  assert(p);
  glColor3f(1, 0, 0);
  glPushMatrix();
  glTranslatef(p->x, p->y, 0);
  draw_unit();
  glPopMatrix();
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
    draw_unit();
    glPopMatrix();
  }
  glPopMatrix();
}

static void draw_unit_at(const V2i *p) {
  V2f f;
  assert(p);
  v2i_to_v2f(&f, p);
  draw_unit_at_f(&f);
}

static void draw_units(void) {
  Node *node;
  FOR_EACH_NODE(units, node) {
    Unit *u = node->data;
    if (unit_mode == UM_MOVING && u == selected_unit) {
      continue;
    }
    draw_unit_at(&u->pos);
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
  fill_map(pos);
  build_walkable_array(&va_walkable_map);
}

static float get_rot_angle(const V2f *a, const V2f *b) {
  float distance, angle;
  V2f diff_2;
  assert(a);
  assert(b);
  diff_2.x = (float)pow(b->x - a->x, 2);
  diff_2.y = (float)pow(b->y - a->y, 2);
  distance = (float)sqrt(diff_2.x + diff_2.y);
  angle = rad2deg((float)asin((b->x - a->x) / distance));
  if (b->y - a->y > 0) {
    angle = -(180 + angle);
  }
  return angle;
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
  glRotatef(90, 0, 0, 1);
  glRotatef(get_rot_angle(&from_f, &to_f), 0, 0, 1);
  glEnable(GL_TEXTURE_2D);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  {
    glBindTexture(GL_TEXTURE_2D, unit_texture);
    glTexCoordPointer(2, GL_FLOAT, 0, va_obj.t);
    glVertexPointer(3, GL_FLOAT, 0, va_obj.v);
    glDrawArrays(GL_TRIANGLES, 0, va_obj.count);
  }
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisable(GL_TEXTURE_2D);
  glDisableClientState(GL_VERTEX_ARRAY);
  glPopMatrix();
  current_move_index++;
  if (current_move_index == last_move_index) {
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
    if (u) {
      selected_unit = u;
      fill_map(&active_tile_pos);
      build_walkable_array(&va_walkable_map);
    } else if (selected_unit) {
      if (t->cost < max_cost && t->parent != D_NONE) {
        fill_map(&selected_unit->pos);
        move_path = get_path(active_tile_pos);
        unit_mode = UM_MOVING;
        current_move_index = 0;
        last_move_index = (move_path.count - 1) * move_speed;
        free(va_walkable_map.v);
        va_walkable_map.v = NULL;
        va_walkable_map.count = 0;
      }

    }
  }
}

static void process_mouse_motion_event(
    const SDL_MouseMotionEvent *e)
{
  assert(e);
  set_v2i(&mouse_pos, (int)e->x, (int)e->y);
#if 0
  V2i v;
  assert(e);
  v.x = e->x / TILE_SIZE;
  v.y = e->y / TILE_SIZE;
  if (inboard(&v)) {
    fill_map(&v);
  }
#endif
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
        fill_map(&selected_unit->pos);
        build_walkable_array(&va_walkable_map);
      }
      break;
    }
    case SDLK_MINUS: {
      action_points -= 1;
      if (selected_unit) {
        fill_map(&selected_unit->pos);
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
    set_rgb(va->ub_c, 4, i, 0, (GLubyte)p.x, (GLubyte)p.y, 1);
    set_rgb(va->ub_c, 4, i, 1, (GLubyte)p.x, (GLubyte)p.y, 1);
    set_rgb(va->ub_c, 4, i, 2, (GLubyte)p.x, (GLubyte)p.y, 1);
    set_rgb(va->ub_c, 4, i, 3, (GLubyte)p.x, (GLubyte)p.y, 1);
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
  glAlphaFunc(GL_GREATER, 0.0f);
}

static void init_camera(void){
  camera.x_angle = 45.0f;
  camera.z_angle = 45.0f;
  camera.pos.x = 0;
  camera.pos.y = 0;
  camera.zoom = 50.0f;
}

static void init_units(void) {
  {
    units.head = NULL;
    units.tail = NULL;
    units.count = 0;
    selected_unit = NULL;
  }
  {
    V2i p1 = {1, 1};
    V2i p2 = {2, 2};
    V2i p3 = {10, 13};
    V2i p4 = {3, 5};
    add_unit(p1);
    add_unit(p2);
    add_unit(p3);
    add_unit(p4);
  }
}

static void init_obstacles(void) {
  V2i p;
  for (set_v2i(&p, 0, 0); inboard(&p); inc_v2i(&p)) {
    Tile *t = tile(&p);
    t->obstacle = ((rand() % 100) > 85);
  }
}

/* TODO */
static void init_ui_opengl(void) {
  srand(time(NULL));
  done = false;
  set_v2i(&win_size, 400, 200);
  set_v2i(&active_tile_pos, 0, 0);
  set_v2i(&mouse_pos, 0, 0);
  SDL_Init(SDL_INIT_EVERYTHING);
  screen = SDL_SetVideoMode(win_size.x, win_size.y,
      32, SDL_OPENGL | SDL_GL_DOUBLEBUFFER);
  init_opengl();
  is_dragging_map = false;
  action_points = 24;
  init_camera();
  obj_read(&test_obj_model, DATA("tank.obj"));
  obj_build(&va_obj, &test_obj_model);
  load_texture(DATA("floor.png"), &floor_texture);
  load_texture(DATA("tank.png"), &unit_texture);
  init_pathfinding_module();
  va_map.v = NULL;
  va_map.ub_c = NULL;
  va_map.count = 0;
  va_pick.v = NULL;
  va_pick.count = 0;
  build_picking_tiles_array(&va_pick);
  clean_map();
  va_walkable_map.v = NULL;
  va_walkable_map.count = 0;
  init_units();
  unit_mode = UM_NORMAL;
  move_speed = 10;
  move_path.head = NULL;
  move_path.tail = NULL;
  move_path.count = 0;
  init_obstacles();
  build_map_array();
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
  UNUSED(draw_line);
  UNUSED(draw_units_at_f);
  do_ui_opengl();
  return 0;
}
