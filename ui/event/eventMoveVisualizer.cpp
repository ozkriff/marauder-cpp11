// See LICENSE file for copyright and license details.

#include "ui/event/eventMoveVisualizer.hpp"
#include <cassert>
#include "SDL_opengl.h"
#include "core/misc.hpp"
#include "core/v2i.hpp"
#include "core/dir.hpp"
#include "core/core.hpp"
#include "core/pathfinder.hpp"
#include "ui/v2f.hpp"
#include "ui/vertexArray.hpp"
#include "ui/game.hpp"
#include "ui/math.hpp"

EventMoveVisualizer::EventMoveVisualizer(Game& game, const Event& event)
  : EventVisualizer(game),
    mEventMove(dynamic_cast<const EventMove&>(event)),
    mCurrentMoveIndex(0)
{
  game.cleanWalkableMapArray();
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

bool EventMoveVisualizer::isUnitVisible(const Unit& u) const {
  return u.id() == mEventMove.unitID();
}

void EventMoveVisualizer::draw() {
  const Unit& unit = game().core().id2unit(mEventMove.unitID());
  V2f pos = currentPos();
  glPushMatrix();
  glTranslatef(pos.x(), pos.y(), 0.0f);
  game().drawUnitCircle(unit);
  glRotatef(currentAngle(), 0, 0, 1);
  game().drawUnitModel(unit);
  glPopMatrix();
  mCurrentMoveIndex++;
}

const V2i& EventMoveVisualizer::currentTile() const {
  return mEventMove.path()[currentTileIndex()];
}

const V2i& EventMoveVisualizer::nextTile() const {
  return mEventMove.path()[currentTileIndex() + 1];
}

void EventMoveVisualizer::endMovement() {
  Core& core = game().core();
  Unit& u = core.id2unit(mEventMove.unitID());
  UNUSED(u);
  if (core.isAnyUnitSelected()) {
    game().rebuildWalkableMapArray();
    game().rebuildMapArray();
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
  return dirToAngle(Dir(currentTile(), nextTile()));
}

V2f EventMoveVisualizer::currentPos() const {
  V2f from = game().v2iToV2f(currentTile());
  V2f to = game().v2iToV2f(nextTile());
  V2f diff = (to - from) / moveSpeed;
  int nodeIndex = calculateNodeIndex();
  return from + (diff * nodeIndex);
}