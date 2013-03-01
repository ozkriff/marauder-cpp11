// See LICENSE file for copyright and license details.

#ifndef VISUALIZER__EVENT__EVENT_MOVE_VISUALIZER_HPP
#define VISUALIZER__EVENT__EVENT_MOVE_VISUALIZER_HPP

#include "core/event/eventMove.hpp"
#include "visualizer/eventVisualizer.hpp"

class Event;

class EventMoveVisualizer : public EventVisualizer {
public:
  EventMoveVisualizer(Visualizer& visualizer, const Event& event);
  virtual ~EventMoveVisualizer();

  virtual bool isFinished() const;
  virtual void draw();
  virtual void end();

private:
  static const int moveSpeed = 10;

  const EventMove& mEventMove;
  int mCurrentMoveIndex;

  int currentTileIndex() const;
  int calculateNodeIndex() const;
  void endMovement();
  const V2i& currentTile() const;
  const V2i& nextTile() const;
  int framesCount() const;
  float currentAngle() const;
  V2f currentPos() const;
};

#endif
