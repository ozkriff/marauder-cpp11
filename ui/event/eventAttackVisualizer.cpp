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
  return u.id() == mEventAttack.victimID();
}

void EventAttackVisualizer::draw() {
  // TODO: animate shooting
  const Unit& u = game().core().id2unit(mEventAttack.victimID());
  V2f posTmp = game().v2iToV2f(u.position());
  V3f pos(posTmp.x(), posTmp.y(), -0.1f * mFrame); // TODO: magic!
  glPushMatrix();
  glTranslatef(pos.x, pos.y, 0.0f);
  glRotatef(u.direction().toAngle() + 120.0f, 0, 0, 1); // TODO: Remove '+ 120'! Rotate obj files!
  game().drawUnitCircle(u);
  glTranslatef(0.0f, 0.0f, pos.z);
  game().drawUnitModel(u);
  glPopMatrix();
  drawLineOfFire();
  mFrame++;
}

void EventAttackVisualizer::end() {
  if (game().core().isAnyUnitSelected()) {
    game().core().pathfinder().fillMap(game().core().selectedUnit());
    game().setVaWalkableMap(game().buildWalkableArray());
  }
}

// private:

void EventAttackVisualizer::drawLineOfFire() {
  const Unit& attacker = game().core().id2unit(mEventAttack.attackerID());
  const Unit& victim = game().core().id2unit(mEventAttack.victimID());
  V2f from = game().v2iToV2f(attacker.position());
  V2f to = game().v2iToV2f(victim.position());
  std::vector<float> v;
  appendV3f(&v, V3f(from.x(), from.y(), 1.0f));
  appendV3f(&v, V3f(
      to.x() + rnd(0, 30) / 10.0f,
      to.y() + rnd(0, 30) / 10.0f,
      (rnd(0, 20) / 10.0f)));
  glLineWidth(rnd(1, 30) / 10.0f);
  glEnableClientState(GL_VERTEX_ARRAY);
  glColor3f(1.0f, 0.0f, 0.0f);
  glVertexPointer(3, GL_FLOAT, 0, v.data());
  glDrawArrays(GL_LINES, 0, v.size() / 3);
  glDisableClientState(GL_VERTEX_ARRAY);
}
