// See LICENSE file for copyright and license details.

#ifndef VISUALIZER__EVENT__EVENT_END_TURN_VISUALIZER_HPP
#define VISUALIZER__EVENT__EVENT_END_TURN_VISUALIZER_HPP

#include "core/event/eventEndTurn.hpp"
#include "visualizer/eventVisualizer.hpp"

class Event;

class EventEndTurnVisualizer : public EventVisualizer {
public:
  EventEndTurnVisualizer(Visualizer& visualizer, const Event& event);
  virtual ~EventEndTurnVisualizer();

  virtual bool isFinished() const;
  virtual void draw();
  virtual void end();

private:
  const EventEndTurn& mEventEndTurn;
};

#endif
