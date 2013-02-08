// See LICENSE file for copyright and license details.

#include "ui/event/eventAttackVisualizer.hpp"
#include <cassert>
#include <SDL/SDL_opengl.h>
#include "core/core.hpp"
#include "core/misc.hpp"
#include "ui/v2f.hpp"
#include "ui/v3f.hpp"
#include "ui/vertexArray.hpp"
#include "ui/game.hpp"

EventAttackVisualizer::EventAttackVisualizer(Game& game, const Event& event)
  : EventVisualizer(game),
    mEventAttack(dynamic_cast<const EventAttack&>(event)),
    mFrame(0)
{
  game.setVaWalkableMap(VertexArray());
}

EventAttackVisualizer::~EventAttackVisualizer() {
}

bool EventAttackVisualizer::isFinished() {
  return mFrame >= 60; // TODO: magic! magic!
}

bool EventAttackVisualizer::isUnitVisible(const Unit& u) {
  return u.id() == mEventAttack.mVictimID;
}

void EventAttackVisualizer::draw() {
  // TODO: animate shooting
  Unit& u = game().core().id2unit(mEventAttack.mVictimID);
  V2f posTmp = game().v2iToV2f(u.position());
  V3f pos(posTmp.x(), posTmp.y(), -0.1f * mFrame); // TODO: magic!
  glPushMatrix();
  glTranslatef(pos.x, pos.y, 0.0f);
  glRotatef(u.direction().toAngle() + 120.0f, 0, 0, 1); // TODO: Remove '+ 120'! Rotate obj files!
  game().drawUnitCircle(u);
  glTranslatef(0.0f, 0.0f, pos.z);
  game().drawUnitModel(u);
  glPopMatrix();
  mFrame++;
}

void EventAttackVisualizer::end() {
  if (game().core().isAnyUnitSelected()) {
    game().core().pathfinder().fillMap(game().core().selectedUnit());
    game().setVaWalkableMap(game().buildWalkableArray());
  }
}
