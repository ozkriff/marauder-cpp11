// See LICENSE file for copyright and license details.

#include "ui/event/eventEndTurnVisualizer.hpp"
#include <cassert>
#include <SDL/SDL_opengl.h>
#include "core/misc.hpp"
#include "core/v2i.hpp"
#include "core/dir.hpp"
#include "core/core.hpp"
#include "core/pathfinder.hpp"
#include "ui/v2f.hpp"
#include "ui/vertexArray.hpp"
#include "ui/game.hpp"

EventEndTurnVisualizer::EventEndTurnVisualizer(Game& game, const Event& event)
  : EventVisualizer(game),
    mEventEndTurn(dynamic_cast<const EventEndTurn&>(event))
{
}

EventEndTurnVisualizer::~EventEndTurnVisualizer() {
}

bool EventEndTurnVisualizer::isFinished() {
  return true;
}

bool EventEndTurnVisualizer::isUnitVisible(const Unit& u) {
  UNUSED(u);
  return false;
}

void EventEndTurnVisualizer::draw() {
  // die("TODO"); // TODO: ...
}

void EventEndTurnVisualizer::end() {
  game().core().calculateFow();
  game().setVaFogOfWar(game().buildFowArray());
  game().setVaWalkableMap(VertexArray());
}
