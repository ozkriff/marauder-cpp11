// See LICENSE file for copyright and license details.

#include "ui/event/eventAttackVisualizer.hpp"
#include <cassert>
#include "SDL_opengl.h"
#include "core/core.hpp"
#include "core/misc.hpp"
#include "ui/v2f.hpp"
#include "ui/v3f.hpp"
#include "ui/vertexArray.hpp"
#include "ui/game.hpp"

EventAttackVisualizer::EventAttackVisualizer(Game& game, const Event& event)
  : EventVisualizer(game),
    mEventAttack(dynamic_cast<const EventAttack&>(event)),
    mFrame(0),
    mLastFrame(60),
    mFallingDownSpeed(0.005f),
    mAttacker(game.core().id2unit(mEventAttack.attackerID())),
    mVictim(game.core().id2unit(mEventAttack.victimID()))
{
  game.cleanWalkableMapArray();
}

EventAttackVisualizer::~EventAttackVisualizer() {
}

bool EventAttackVisualizer::isFinished() const {
  return mFrame >= mLastFrame;
}

bool EventAttackVisualizer::isUnitVisible(const Unit& u) const {
  return u.id() == mEventAttack.victimID();
}

void EventAttackVisualizer::draw() {
  // TODO: animate shooting
  V2f posTmp = game().v2iToV2f(mVictim.position());
  V3f pos(posTmp, -mFallingDownSpeed * mFrame);
  glPushMatrix();
  glTranslatef(pos.x(), pos.y(), 0.0f);
  game().drawUnitCircle(mVictim);
  glRotatef(mVictim.direction().toAngle(), 0, 0, 1);
  glTranslatef(0.0f, 0.0f, pos.z());
  game().drawUnitModel(mVictim);
  glPopMatrix();
  drawLineOfFire();
  mFrame++;
}

void EventAttackVisualizer::end() {
  if (game().core().isAnyUnitSelected()) {
    game().rebuildWalkableMapArray();
    game().rebuildMapArray();
  }
}

// private:

void EventAttackVisualizer::drawLineOfFire() {
  V2f from = game().v2iToV2f(mAttacker.position());
  V2f to = game().v2iToV2f(mVictim.position());
  std::vector<float> v;
  appendV3f(&v, V3f(from, 0.2f));
  appendV3f(&v, V3f(
      to.x() + (rnd(-30, 30) / 100.0f),
      to.y() + (rnd(-30, 30) / 100.0f),
      0));
  glLineWidth(rnd(1, 30) / 10.0f);
  glEnableClientState(GL_VERTEX_ARRAY);
  glColor3f(1.0f, 0.0f, 0.0f);
  glVertexPointer(3, GL_FLOAT, 0, v.data());
  glDrawArrays(GL_LINES, 0, v.size() / 3);
  glDisableClientState(GL_VERTEX_ARRAY);
}
