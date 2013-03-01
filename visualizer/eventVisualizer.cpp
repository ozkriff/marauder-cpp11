// See LICENSE file for copyright and license details.

#include <cassert>
#include <stdexcept>
#include "SDL.h"
#include "SDL_opengl.h"
#include "core/v2i.hpp"
#include "core/dir.hpp"
#include "core/core.hpp"
#include "core/misc.hpp"
#include "core/event.hpp"
#include "visualizer/v2f.hpp"
#include "visualizer/v3f.hpp"
#include "visualizer/math.hpp"
#include "visualizer/vertexArray.hpp"
#include "visualizer/eventVisualizer.hpp"
#include "visualizer/event/eventMoveVisualizer.hpp"
#include "visualizer/event/eventEndTurnVisualizer.hpp"
#include "visualizer/event/eventAttackVisualizer.hpp"

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
