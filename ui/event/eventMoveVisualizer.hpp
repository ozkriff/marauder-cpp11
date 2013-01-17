// See LICENSE file for copyright and license details.

#ifndef UI__EVENT__EVENT_MOVE_VISUALIZER_HPP
#define UI__EVENT__EVENT_MOVE_VISUALIZER_HPP

#include "core/event/eventMove.hpp"
#include "ui/eventVisualizer.hpp"

class Event;

class EventMoveVisualizer : public EventVisualizer {
private:
  static const int moveSpeed = 10;

  const EventMove& mEventMove;
  int mCurrentMoveIndex;

  int getNodeIndex();
  void endMovement();
  void getCurrentMovingNodes(V2i* from, V2i* to);
  int framesCount();

public:
  EventMoveVisualizer(Game& game, const Event& event);
  virtual ~EventMoveVisualizer();

  virtual bool isFinished();
  virtual bool isUnitVisible(const Unit& u);
  virtual void draw();
  virtual void end();
};

#endif
