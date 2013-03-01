// See LICENSE file for copyright and license details.

#include "visualizer/event/eventEndTurnVisualizer.hpp"
#include <cassert>
#include "SDL_opengl.h"
#include "core/misc.hpp"
#include "core/v2i.hpp"
#include "core/dir.hpp"
#include "core/core.hpp"
#include "core/pathfinder.hpp"
#include "visualizer/v2f.hpp"
#include "visualizer/vertexArray.hpp"
#include "visualizer/visualizer.hpp"

EventEndTurnVisualizer::EventEndTurnVisualizer(Visualizer& visualizer, const Event& event)
  : EventVisualizer(visualizer),
    mEventEndTurn(dynamic_cast<const EventEndTurn&>(event))
{
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
