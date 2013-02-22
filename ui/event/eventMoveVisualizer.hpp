// See LICENSE file for copyright and license details.

#ifndef UI__EVENT__EVENT_MOVE_VISUALIZER_HPP
#define UI__EVENT__EVENT_MOVE_VISUALIZER_HPP

#include "core/event/eventMove.hpp"
#include "ui/eventVisualizer.hpp"

class Event;

class EventMoveVisualizer : public EventVisualizer {
public:
  EventMoveVisualizer(Game& game, const Event& event);
  virtual ~EventMoveVisualizer();

  virtual bool isFinished();
  virtual bool isUnitVisible(const Unit& u);
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
