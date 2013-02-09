// See LICENSE file for copyright and license details.

#ifndef UI__EVENT__EVENT_END_TURN_VISUALIZER_HPP
#define UI__EVENT__EVENT_END_TURN_VISUALIZER_HPP

#include "core/event/eventEndTurn.hpp"
#include "ui/eventVisualizer.hpp"

class Event;

class EventEndTurnVisualizer : public EventVisualizer {
public:
  EventEndTurnVisualizer(Game& game, const Event& event);
  virtual ~EventEndTurnVisualizer();

  virtual bool isFinished();
  virtual bool isUnitVisible(const Unit& u);
  virtual void draw();
  virtual void end();

private:
  const EventEndTurn& mEventEndTurn;
};

#endif
