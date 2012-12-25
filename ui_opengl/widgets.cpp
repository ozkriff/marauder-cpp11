/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include <list>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_opengl.h>
#include "core/v2i.h"
#include "core/dir.h"
#include "core/misc.h"
#include "ui_opengl/v2f.h"
#include "ui_opengl/math.h"
#include "ui_opengl/gl.h"
#include "ui_opengl/vertex_array.h"
#include "ui_opengl/widgets.h"
#include "ui_opengl/config.h"

std::list<Button*> buttons;

GLuint ttf_gl_print(TTF_Font *f, const char *text, V2i *size) {
  SDL_Color Color = {255, 255, 255, 0};
  SDL_Surface *s = TTF_RenderUTF8_Blended(f, text, Color);
  GLuint id;
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameterf(GL_TEXTURE_2D,
      GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D,
      GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h,
      0, GL_BGRA, GL_UNSIGNED_BYTE, s->pixels);
  size->x = s->w;
  size->y = s->h;
  SDL_FreeSurface(s);
  return id;
}

void change_button_text(Button *b, char *text) {
  glDeleteTextures(1, &b->texture_id);
  b->texture_id = ttf_gl_print(b->f, text, &b->size);
}

void change_button_text_by_id(int id, char *text) {
  for (auto b : buttons) {
    if (b->id == id) {
      change_button_text(b, text);
      return;
    }
  }
}

static int get_new_button_id() {
  if (buttons.size() > 0) {
    return buttons.back()->id + 1;
  } else {
    return 0;
  }
}

int add_button(TTF_Font *f, const V2i *pos,
    const char *text, ButtonCallback callback)
{
  auto b = new Button;
  buttons.push_back(b);
  assert(text);
  assert(f);
  assert(pos);
  b->f = f;
  int id = get_new_button_id();
  b->id = id;
  b->pos = *pos;
  b->text = strdup(text);
  b->texture_id = ttf_gl_print(f, text, &b->size);
  b->callback = callback;
  return id;
}

Button* v2i_to_button(V2i pos) {
  for (auto b : buttons) {
    if (pos.x >= b->pos.x
        && pos.y >= b->pos.y
        && pos.x <= b->pos.x + b->size.x
        && pos.y <= b->pos.y + b->size.y) {
      return b;
    }
  }
  return nullptr;
}

void draw_button(Button *b) {
  float rect[4 * 2];
  float texture_coord[4 * 2];
  set_xy(rect, 4, 0, 0, 0, static_cast<float>(b->size.y));
  set_xy(rect, 4, 0, 1,
      static_cast<float>(b->size.x), static_cast<float>(b->size.y));
  set_xy(rect, 4, 0, 2, static_cast<float>(b->size.x), 0);
  set_xy(rect, 4, 0, 3, 0, 0);
  set_xy(texture_coord, 4, 0, 0, 0, 1);
  set_xy(texture_coord, 4, 0, 1, 1, 1);
  set_xy(texture_coord, 4, 0, 2, 1, 0);
  set_xy(texture_coord, 4, 0, 3, 0, 0);
  glPushMatrix();
  glTranslatef(static_cast<float>(b->pos.x),
      static_cast<float>(b->pos.y), 0.0f);
  glColor4f(0.3f, 0.3f, 0.3f, 0.6f);
  glVertexPointer(2, GL_FLOAT, 0, rect);
  glDrawArrays(GL_QUADS, 0, 4);
  glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D, b->texture_id);
  glVertexPointer(2, GL_FLOAT, 0, rect);
  glTexCoordPointer(2, GL_FLOAT, 0, texture_coord);
  glDrawArrays(GL_QUADS, 0, 4);
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
}

static void set_2d_widgets_drawing_mode_on() {
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, WIN_WIDTH, WIN_HEIGHT, 0, 0, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
}

static void set_2d_widgets_drawing_mode_off() {
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glEnable(GL_DEPTH_TEST);
}

void draw_buttons() {
  set_2d_widgets_drawing_mode_on();
  for (auto b : buttons) {
    draw_button(b);
  }
  set_2d_widgets_drawing_mode_off();
}

TTF_Font* open_font(const char *font_name, int size) {
  TTF_Font *f = TTF_OpenFont(font_name, size);
  if (!f) {
    die("open_font(): "
        "Unable to open font file: '%s'\n",
        TTF_GetError());
  }
  return f;
}

void init_widgets() {
  TTF_Init();
  // buttons = empty_list;
}
