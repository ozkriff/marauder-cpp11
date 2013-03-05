// See LICENSE file for copyright and license details.

#include "visualizer/event/eventEndTurnVisualizer.hpp"
#include "core/core.hpp"
#include "visualizer/vertexArray.hpp"

EventEndTurnVisualizer::EventEndTurnVisualizer(Visualizer& visualizer, const EventEndTurnView& event)
  : EventVisualizer(visualizer),
    mEventEndTurn(event)
{
  UNUSED(mEventEndTurn);
}

EventEndTurnVisualizer::~EventEndTurnVisualizer() {
}

bool EventEndTurnVisualizer::isFinished() const {
  return true;
}

void EventEndTurnVisualizer::draw() {
  // nothing to draw...
}

void EventEndTurnVisualizer::end() {
  visualizer().core().calculateFow();
  visualizer().rebuildMapArray();
  visualizer().cleanWalkableMapArray();
  visualizer().recreateUnitSceneNodes();
}
