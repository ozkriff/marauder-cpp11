// See LICENSE file for copyright and license details.

#include <cassert>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include "core/v2i.hpp"
#include "core/dir.hpp"
#include "core/core.hpp"
#include "core/misc.hpp"
#include "core/event.hpp"
#include "ui/v2f.hpp"
#include "ui/v3f.hpp"
#include "ui/math.hpp"
#include "ui/vertex_array.hpp"
#include "ui/event.hpp"
#include "ui/event/move.hpp"
#include "ui/event/end_turn.hpp"

EventVisualizer::EventVisualizer(Game& game)
  : mGame(game)
{
}

EventVisualizer::~EventVisualizer() {
}

const Game& EventVisualizer::game() const {
  return mGame;
}

Game& EventVisualizer::game() {
  return mGame;
}

EventVisualizer* newEventVisualizer(Game& game, const Event& event) {
  switch (event.t) {
  case EventTypeID::MOVE:
    return new EventMoveVisualizer(game, event);
  case EventTypeID::END_TURN:
    return new EventEndTurnVisualizer(game, event);
  default:
    return nullptr;
  }
}
