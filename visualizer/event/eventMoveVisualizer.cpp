// See LICENSE file for copyright and license details.

#include "visualizer/event/eventMoveVisualizer.hpp"
#include <cassert>
#include "core/core.hpp"
#include "core/eventView.hpp"
#include "visualizer/vertexArray.hpp"
#include "visualizer/math.hpp"

EventMoveVisualizer::EventMoveVisualizer(Visualizer& visualizer, const EventMoveView& event)
  : EventVisualizer(visualizer),
    mEventMove(event),
    mCurrentMoveIndex(0)
{
  visualizer.cleanWalkableMapArray();
}

EventMoveVisualizer::~EventMoveVisualizer() {
}

int EventMoveVisualizer::framesCount() const {
  return (mEventMove.path().size() - 1) * moveSpeed;
}

bool EventMoveVisualizer::isFinished() const {
  assert(mCurrentMoveIndex <= framesCount());
  return (mCurrentMoveIndex == framesCount());
}

void EventMoveVisualizer::draw() {
  const Unit& unit = visualizer().core().id2unit(mEventMove.unitID());
  auto& node = visualizer().sceneManager().sceneNode(unit.id());
  node.mPosition = currentPosition();
  node.mRotationAngle = currentAngle();
  ++mCurrentMoveIndex;
}

const V2i& EventMoveVisualizer::currentTile() const {
  return mEventMove.path()[currentTileIndex()];
}

const V2i& EventMoveVisualizer::nextTile() const {
  return mEventMove.path()[currentTileIndex() + 1];
}

void EventMoveVisualizer::endMovement() {
  Core& core = visualizer().core();
  Unit& u = core.id2unit(mEventMove.unitID());
  auto& node = visualizer().sceneManager().sceneNode(u.id());
  node.mPosition = visualizer().v2iToV2f(mEventMove.path().back());
  node.mRotationAngle = dirToAngle(u.direction());
  if (core.isAnyUnitSelected()) {
    visualizer().rebuildWalkableMapArray();
    visualizer().rebuildMapArray();
  }
}

int EventMoveVisualizer::currentTileIndex() const {
  return mCurrentMoveIndex / moveSpeed;
}

int EventMoveVisualizer::calculateNodeIndex() const {
  return mCurrentMoveIndex - currentTileIndex() * moveSpeed;
}

void EventMoveVisualizer::end() {
  endMovement();
}

float EventMoveVisualizer::currentAngle() const {
  return dirToAngle(Direction(currentTile(), nextTile()));
}

V2f EventMoveVisualizer::currentPosition() const {
  V2f from = visualizer().v2iToV2f(currentTile());
  V2f to = visualizer().v2iToV2f(nextTile());
  V2f diff = (to - from) / moveSpeed;
  int nodeIndex = calculateNodeIndex();
  return from + (diff * nodeIndex);
}
