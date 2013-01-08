// See LICENSE file for copyright and license details.

#ifndef UI_OPENGL_EVENT_H
#define UI_OPENGL_EVENT_H

#include "ui_opengl/game.hpp"

int getLastEventIndex(Game& game, const Event& e);
bool eventFilterUnit(Game& game, const Event& e, const Unit& u);
void eventDraw(Game& game, const Event& e);

#endif
