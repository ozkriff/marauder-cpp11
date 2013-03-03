// See LICENSE file for copyright and license details.

#include "visualizer/event/eventAttackVisualizer.hpp"
#include <cassert>
#include "core/misc.hpp"
#include "visualizer/vertexArray.hpp"
#include "visualizer/visualizer.hpp"
#include "visualizer/math.hpp"

EventAttackVisualizer::EventAttackVisualizer(Visualizer& visualizer, const Event& event)
  : EventVisualizer(visualizer),
    mEventAttack(dynamic_cast<const EventAttack&>(event)),
    mFrame(0),
    mLastFrame(60),
    mFallingDownSpeed(0.005f),
    mAttacker(visualizer.core().id2unit(mEventAttack.attackerID())),
    mVictim(visualizer.core().id2unit(mEventAttack.victimID()))
{
  visualizer.cleanWalkableMapArray();
}

EventAttackVisualizer::~EventAttackVisualizer() {
}

bool EventAttackVisualizer::isFinished() const {
  return mFrame >= mLastFrame;
}

void EventAttackVisualizer::draw() {
  auto& node = visualizer().sceneManager().sceneNode(mVictim.id());
  node.mPosition.setZ(node.mPosition.z() - mFallingDownSpeed);
  drawLineOfFire();
  ++mFrame;
}

void EventAttackVisualizer::end() {
  visualizer().sceneManager().deleteNode(mVictim.id());
  if (visualizer().core().isAnyUnitSelected()) {
    visualizer().rebuildWalkableMapArray();
    visualizer().rebuildMapArray();
  }
}

// private:

void EventAttackVisualizer::drawLineOfFire() {
  V2f from = visualizer().v2iToV2f(mAttacker.position());
  V2f to = visualizer().v2iToV2f(mVictim.position());
  VertexArray v(Color(1.0f, 0.0f, 0.0f), PrimitiveType::Lines);
  appendV3f(&v.vertices, V3f(from, 0.2f));
  appendV3f(&v.vertices, V3f(
      to.x() + (rnd(-30, 30) / 100.0f),
      to.y() + (rnd(-30, 30) / 100.0f),
      0));
  glLineWidth(rnd(1, 30) / 10.0f);
  v.draw();
  glLineWidth(1);
}
