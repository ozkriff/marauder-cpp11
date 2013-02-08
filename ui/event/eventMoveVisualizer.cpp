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
  game.setVaWalkableMap(VertexArray());
}

EventMoveVisualizer::~EventMoveVisualizer() {
}

int EventMoveVisualizer::framesCount() {
  return (mEventMove.path.size() - 1) * moveSpeed;
}

bool EventMoveVisualizer::isFinished() {
  assert(mCurrentMoveIndex <= framesCount());
  return (mCurrentMoveIndex == framesCount());
}

bool EventMoveVisualizer::isUnitVisible(const Unit& u) {
  return u.id() == mEventMove.unitID;
}

void EventMoveVisualizer::draw() {
  Unit& u = game().core().id2unit(mEventMove.unitID);
  V2i fromI = currentTile();
  V2i toI = nextTile();
  V2f fromF = game().v2iToV2f(fromI);
  V2f toF = game().v2iToV2f(toI);
  int nodeIndex = calculateNodeIndex();
  V2f diff = (toF - fromF) / moveSpeed;
  V2f p = fromF + diff * nodeIndex;
  glPushMatrix();
  glTranslatef(p.x(), p.y(), 0.0f);
  // TODO: Remove '+ 4'! Rotate obj files!
  glRotatef(
      Dir(fromI, toI).toAngle() + 120.0f,
      0, 0, 1);
  game().drawUnitModel(u);
  game().drawUnitCircle(u);
  glPopMatrix();
  mCurrentMoveIndex++;
}

const V2i& EventMoveVisualizer::currentTile() {
  return mEventMove.path[currentTileIndex()];
}

const V2i& EventMoveVisualizer::nextTile() {
  return mEventMove.path[currentTileIndex() + 1];
}

void EventMoveVisualizer::endMovement() {
  Core& core = game().core();
  Unit& u = core.id2unit(mEventMove.unitID);
  u.setPosition(mEventMove.path.back());
  if (core.isAnyUnitSelected()) {
    core.pathfinder().fillMap(u);
    game().setVaWalkableMap(game().buildWalkableArray());
    core.calculateFow();
    game().setVaFogOfWar(game().buildFowArray());
  }
  if (u.playerID() == core.currentPlayer().id) {
    if (core.isAnyUnitSelected()) {
      core.pathfinder().fillMap(core.selectedUnit());
      game().setVaWalkableMap(game().buildWalkableArray());
    }
    game().setVaFogOfWar(game().buildFowArray());
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
