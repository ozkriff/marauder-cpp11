// See LICENSE file for copyright and license details.

#ifndef UI_OPENGL_EVENT_END_TURN_H
#define UI_OPENGL_EVENT_END_TURN_H

#include "core/event/end_turn.hpp"
#include "ui/event.hpp"

class Event;

class EventEndTurnVisualizer : public EventVisualizer {
private:
  const EventEndTurn& mEventEndTurn;

public:
  EventEndTurnVisualizer(Game& game, const Event& event);
  virtual ~EventEndTurnVisualizer();

  virtual int framesCount();
  virtual bool filterUnit(const Unit& u);
  virtual void draw();
  virtual void end();
};

#endif
