// See LICENSE file for copyright and license details.

#include <cassert>
#include "SDL.h"
#include "SDL_opengl.h"
#include "core/v2i.hpp"
#include "core/dir.hpp"
#include "core/core.hpp"
#include "core/misc.hpp"
#include "core/event.hpp"
#include "ui/v2f.hpp"
#include "ui/v3f.hpp"
#include "ui/math.hpp"
#include "ui/vertexArray.hpp"
#include "ui/eventVisualizer.hpp"
#include "ui/event/eventMoveVisualizer.hpp"
#include "ui/event/eventEndTurnVisualizer.hpp"
#include "ui/event/eventAttackVisualizer.hpp"

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
  switch (event.type()) {
  case EventTypeID::MOVE:
    return new EventMoveVisualizer(game, event);
  case EventTypeID::END_TURN:
    return new EventEndTurnVisualizer(game, event);
  case EventTypeID::ATTACK:
    return new EventAttackVisualizer(game, event);
  default:
    throw std::logic_error("default case!");
  }
}
