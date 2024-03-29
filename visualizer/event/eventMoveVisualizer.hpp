// See LICENSE file for copyright and license details.

#ifndef VISUALIZER__EVENT__EVENT_MOVE_VISUALIZER_HPP
#define VISUALIZER__EVENT__EVENT_MOVE_VISUALIZER_HPP

#include "core/event/eventMove.hpp"
#include "visualizer/eventVisualizer.hpp"

class EventMoveView;

class EventMoveVisualizer : public EventVisualizer {
public:
  EventMoveVisualizer(Visualizer& visualizer, const EventMoveView& eventView);
  virtual ~EventMoveVisualizer();

  virtual bool isFinished() const override;
  virtual void draw() override;
  virtual void end() override;

private:
  static const int moveSpeed = 10;

  const EventMoveView& mEventMove;
  int mCurrentMoveIndex;

  int currentTileIndex() const;
  int calculateNodeIndex() const;
  void endMovement();
  const V2i& currentTile() const;
  const V2i& nextTile() const;
  int framesCount() const;
  float currentAngle() const;
  V2f currentPosition() const;
};

#endif
