// See LICENSE file for copyright and license details.

#include "visualizer/eventVisualizer.hpp"
#include <stdexcept>
#include "core/core.hpp"
#include "core/eventView.hpp"
#include "visualizer/math.hpp"
#include "visualizer/mesh.hpp"
#include "visualizer/event/eventMoveVisualizer.hpp"
#include "visualizer/event/eventEndTurnVisualizer.hpp"
#include "visualizer/event/eventAttackVisualizer.hpp"

EventVisualizer::EventVisualizer(Visualizer& visualizer)
  : mVisualizer(visualizer)
{
}

EventVisualizer::~EventVisualizer() {
}

const Visualizer& EventVisualizer::visualizer() const {
  return mVisualizer;
}

Visualizer& EventVisualizer::visualizer() {
  return mVisualizer;
}

EventVisualizer* newEventVisualizer(
    Visualizer& visualizer, const EventView& eventView)
{
  switch (eventView.type()) {
  case EventViewType::Move:
    return new EventMoveVisualizer(visualizer,
        dynamic_cast<const EventMoveView&>(eventView));
  case EventViewType::EndTurn:
    return new EventEndTurnVisualizer(visualizer,
        dynamic_cast<const EventEndTurnView&>(eventView));
  case EventViewType::Attack:
    return new EventAttackVisualizer(visualizer,
        dynamic_cast<const EventAttackView&>(eventView));
  default:
    throw std::logic_error("default case!");
  }
}
