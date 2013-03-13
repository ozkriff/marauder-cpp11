// See LICENSE file for copyright and license details.

#ifndef VISUALIZER__EVENT__EVENT_END_TURN_VISUALIZER_HPP
#define VISUALIZER__EVENT__EVENT_END_TURN_VISUALIZER_HPP

#include "core/event/eventEndTurn.hpp"
#include "visualizer/eventVisualizer.hpp"

class EventEndTurnView;

class EventEndTurnVisualizer : public EventVisualizer {
public:
  EventEndTurnVisualizer(Visualizer& visualizer, const EventEndTurnView& event);
  virtual ~EventEndTurnVisualizer();

  virtual bool isFinished() const override;
  virtual void draw() override;
  virtual void end() override;

private:
  const EventEndTurnView& mEventEndTurn;
};

#endif
