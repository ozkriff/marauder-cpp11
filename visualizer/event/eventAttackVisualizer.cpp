// See LICENSE file for copyright and license details.

#include "visualizer/event/eventAttackVisualizer.hpp"
#include <cassert>
#include "core/misc.hpp"
#include "core/math.hpp"
#include "core/eventView.hpp"
#include "visualizer/mesh.hpp"
#include "visualizer/visualizer.hpp"
#include "visualizer/math.hpp"

EventAttackVisualizer::EventAttackVisualizer(Visualizer& visualizer, const EventAttackView& event)
  : EventVisualizer(visualizer),
    mEventAttack(event),
    mFrame(0),
    mLastFrame(60),
    mFallingDownSpeed(0.005f)
{
  visualizer.cleanWalkableMapArray();
  {
    auto& sm = visualizer.sceneManager();
    const auto& attackerNode = sm.sceneNode(mEventAttack.attackerID());
    const auto& victimNode = sm.sceneNode(mEventAttack.victimID());
    mInitialAttackerPosition = attackerNode.mPosition;
    mInitialVictimPosition = victimNode.mPosition;
  }
}

EventAttackVisualizer::~EventAttackVisualizer() {
}

bool EventAttackVisualizer::isFinished() const {
  return mFrame >= mLastFrame;
}

void EventAttackVisualizer::draw() {
  auto& node = visualizer().sceneManager().sceneNode(mEventAttack.victimID());
  node.mPosition.setZ(node.mPosition.z() - mFallingDownSpeed);
  drawLineOfFire();
  ++mFrame;
}

void EventAttackVisualizer::end() {
  visualizer().sceneManager().deleteNode(mEventAttack.victimID());
  if (visualizer().core().isAnyUnitSelected()) {
    visualizer().rebuildWalkableMapArray();
    visualizer().rebuildMapArray();
  }
}

// private:

void EventAttackVisualizer::drawLineOfFire() {
  Mesh mesh(Color(1.0f, 0.0f, 0.0f), PrimitiveType::Lines);
  const V3f& from = mInitialAttackerPosition;
  const V3f& to = mInitialVictimPosition;
  float h = 0.2f;
  mesh.addVertex(V3f(from.x(), from.y(), from.z() + h));
  mesh.addVertex(V3f(
      to.x() + (rnd(-30, 30) / 100.0f),
      to.y() + (rnd(-30, 30) / 100.0f)));
  glLineWidth(rnd(1, 30) / 10.0f);
  mesh.draw();
  glLineWidth(1);
}
