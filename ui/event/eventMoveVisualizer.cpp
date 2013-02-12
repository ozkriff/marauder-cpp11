// See LICENSE file for copyright and license details.

#include "ui/event/eventMoveVisualizer.hpp"
#include <cassert>
#include <SDL/SDL_opengl.h>
#include "core/misc.hpp"
#include "core/v2i.hpp"
#include "core/dir.hpp"
#include "core/core.hpp"
#include "core/pathfinder.hpp"
#include "ui/v2f.hpp"
#include "ui/vertexArray.hpp"
#include "ui/game.hpp"

EventMoveVisualizer::EventMoveVisualizer(Game& game, const Event& event)
  : EventVisualizer(game),
    mEventMove(dynamic_cast<const EventMove&>(event)),
    mCurrentMoveIndex(0)
{
  game.cleanWalkableMapArray();
}

EventMoveVisualizer::~EventMoveVisualizer() {
}

int EventMoveVisualizer::framesCount() {
  return (mEventMove.path().size() - 1) * moveSpeed;
}

bool EventMoveVisualizer::isFinished() {
  assert(mCurrentMoveIndex <= framesCount());
  return (mCurrentMoveIndex == framesCount());
}

bool EventMoveVisualizer::isUnitVisible(const Unit& u) {
  return u.id() == mEventMove.unitID();
}

void EventMoveVisualizer::draw() {
  const Unit& unit = game().core().id2unit(mEventMove.unitID());
  V2f pos = currentPos();
  glPushMatrix();
  glTranslatef(pos.x(), pos.y(), 0.0f);
  glRotatef(currentAngle(), 0, 0, 1);
  game().drawUnitModel(unit);
  game().drawUnitCircle(unit);
  glPopMatrix();
  mCurrentMoveIndex++;
}

const V2i& EventMoveVisualizer::currentTile() {
  return mEventMove.path()[currentTileIndex()];
}

const V2i& EventMoveVisualizer::nextTile() {
  return mEventMove.path()[currentTileIndex() + 1];
}

void EventMoveVisualizer::endMovement() {
  Core& core = game().core();
  Unit& u = core.id2unit(mEventMove.unitID());
  u.setPosition(mEventMove.path().back());
  if (core.isAnyUnitSelected()) {
    core.pathfinder().fillMap(u);
    game().rebuildWalkableMapArray();
    core.calculateFow();
    game().rebuildMapArray();
  }
  if (u.playerID() == core.currentPlayer().id) {
    if (core.isAnyUnitSelected()) {
      core.pathfinder().fillMap(core.selectedUnit());
      game().rebuildWalkableMapArray();
    }
    game().rebuildMapArray();
  }
}

int EventMoveVisualizer::currentTileIndex() {
  return mCurrentMoveIndex / moveSpeed;
}

int EventMoveVisualizer::calculateNodeIndex() {
  return mCurrentMoveIndex - currentTileIndex() * moveSpeed;
}

void EventMoveVisualizer::end() {
  endMovement();
}

float EventMoveVisualizer::currentAngle() {
  // TODO: Remove '+ 120.0f'! Rotate models in obj files instead!
  return Dir(currentTile(), nextTile()).toAngle() + 120.0f;
}

V2f EventMoveVisualizer::currentPos() {
  V2f from = game().v2iToV2f(currentTile());
  V2f to = game().v2iToV2f(nextTile());
  V2f diff = (to - from) / moveSpeed;
  int nodeIndex = calculateNodeIndex();
  return from + (diff * nodeIndex);
}