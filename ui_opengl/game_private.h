/* See LICENSE file for copyright and license details. */

#ifndef UI_OPENGL_GAME_PRIVATE_H
#define UI_OPENGL_GAME_PRIVATE_H

/* User interface */

#include "game.h"

typedef enum {
  UI_MODE_NORMAL,
  UI_MODE_SHOW_EVENT
} UIMode;

extern UIMode ui_mode;
extern int last_move_index;
extern int current_move_index;
extern VertexArray va_walkable_map;
extern VertexArray va_fog_of_war;

extern Game game;

#endif
