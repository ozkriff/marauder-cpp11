// See LICENSE file for copyright and license details.

#include "ui/event/end_turn.hpp"
#include <cassert>
#include <SDL/SDL_opengl.h>
#include "core/misc.hpp"
#include "core/v2i.hpp"
#include "core/dir.hpp"
#include "core/core.hpp"
#include "core/path.hpp"
#include "ui/v2f.hpp"
#include "ui/vertex_array.hpp"
#include "ui/game.hpp"

EventEndTurnVisualizer::EventEndTurnVisualizer(Game& game, const Event& event)
  : EventVisualizer(game),
    mEventEndTurn(dynamic_cast<const EventEndTurn&>(event))
{
}

EventEndTurnVisualizer::~EventEndTurnVisualizer() {
}

int EventEndTurnVisualizer::lastIndex() {
  return 0;
}

bool EventEndTurnVisualizer::filterUnit(const Unit& u) {
  UNUSED(u);
  return false;
}

void EventEndTurnVisualizer::draw() {
  die("TODO");
}
