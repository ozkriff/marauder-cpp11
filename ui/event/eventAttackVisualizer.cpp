// See LICENSE file for copyright and license details.

#include "ui/event/eventAttackVisualizer.hpp"
#include <cassert>
#include <SDL/SDL_opengl.h>
#include "core/core.hpp"
#include "core/misc.hpp"
#include "ui/v2f.hpp"
#include "ui/vertexArray.hpp"
#include "ui/game.hpp"

EventAttackVisualizer::EventAttackVisualizer(Game& game, const Event& event)
  : EventVisualizer(game),
    mEventAttack(dynamic_cast<const EventAttack&>(event))
{
  game.setVaWalkableMap(VertexArray());
}

EventAttackVisualizer::~EventAttackVisualizer() {
}

bool EventAttackVisualizer::isFinished() {
  return true;
}

bool EventAttackVisualizer::isUnitVisible(const Unit& u) {
  UNUSED(u);
  return true;
}

void EventAttackVisualizer::draw() {
  // TODO: animate shooting
  // TODO: animate unit moveing underground
}

void EventAttackVisualizer::end() {
}
