// See LICENSE file for copyright and license details.

#ifndef UI_OPENGL_EVENT_MOVE_H
#define UI_OPENGL_EVENT_MOVE_H

#include "core/event/move.h"

class Game;
class Event;

const int moveSpeed = 10;

int getLastEventMoveIndex(Game& game, const Event& e);
void getCurrentMovingNodes(
    Game& game, const EventMove& e, V2i* from, V2i* to);
void drawMovingUnit(Game& game, const EventMove& e);

#endif
