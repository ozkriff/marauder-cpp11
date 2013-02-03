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
  V2i fromI, toI;
  getCurrentMovingNodes(&fromI, &toI);
  V2f fromF = game().v2iToV2f(fromI);
  V2f toF = game().v2iToV2f(toI);
  int nodeIndex = getNodeIndex();
  V2f diff = (toF - fromF) / moveSpeed;
  V2f p = fromF + diff * nodeIndex;
  glPushMatrix();
  glTranslatef(p.x(), p.y(), 0.0f);
  // TODO: Remove '+ 4'! Rotate obj files!
  glRotatef(
      Dir(fromI, toI).toInt() * 60.0f + 120.0f,
      0, 0, 1);
  game().drawUnitModel(u);
  game().drawUnitCircle(u);
  glPopMatrix();
  mCurrentMoveIndex++;
}

void EventMoveVisualizer::getCurrentMovingNodes(V2i* from, V2i* to) {
  int i = mCurrentMoveIndex;
  auto& p = mEventMove.path; // shortcut
  unsigned int j; // node id
  assert(from);
  assert(to);
  for (j = 0; j < p.size() - 2; j++) {
    i -= moveSpeed;
    if (i < 0) {
      break;
    }
  }
  *from = p[j];
  *to = p[j + 1];
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

int EventMoveVisualizer::getNodeIndex() {
  int last = 0;
  int current = 0;
  for (unsigned int j = 0; j < mEventMove.path.size() - 2; j++) {
    current += moveSpeed;
    if (current > mCurrentMoveIndex) {
      break;
    }
    last = current;
  }
  return mCurrentMoveIndex - last;
}

void EventMoveVisualizer::end() {
  endMovement();
}
