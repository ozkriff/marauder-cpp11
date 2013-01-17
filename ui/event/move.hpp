// See LICENSE file for copyright and license details.

#ifndef UI_OPENGL_EVENT_MOVE_H
#define UI_OPENGL_EVENT_MOVE_H

#include "core/event/move.hpp"
#include "ui/event.hpp"

class Event;

class EventMoveVisualizer : public EventVisualizer {
private:
  static const int moveSpeed = 10;

  const EventMove& mEventMove;

  int getNodeIndex();
  void endMovement(const V2i& pos);
  void getCurrentMovingNodes(V2i* from, V2i* to);

public:
  EventMoveVisualizer(Game& game, const Event& event);
  virtual ~EventMoveVisualizer();

  virtual int framesCount();
  virtual bool filterUnit(const Unit& u);
  virtual void draw();
};

#endif
