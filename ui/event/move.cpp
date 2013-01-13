// See LICENSE file for copyright and license details.

#include "ui/event/move.hpp"
#include <cassert>
#include <SDL/SDL_opengl.h>
#include "core/misc.hpp"
#include "core/v2i.hpp"
#include "core/dir.hpp"
#include "core/core.hpp"
#include "core/path.hpp"
#include "ui/v2f.hpp"
#include "ui/vertex_array.hpp"
#include "ui/game.hpp"

EventMoveVisualizer::EventMoveVisualizer(Game& game, const Event& event)
  : EventVisualizer(game),
    mEventMove(dynamic_cast<const EventMove&>(event))
{
}

EventMoveVisualizer::~EventMoveVisualizer() {
}

int EventMoveVisualizer::lastIndex() {
  return (mEventMove.path.size() - 1) * moveSpeed;
}

bool EventMoveVisualizer::filterUnit(const Unit& u) {
  return u.id == mEventMove.unitID;
}

void EventMoveVisualizer::draw() {
  Unit* u = game().core().id2unit(mEventMove.unitID);
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
  game().drawUnitModel(*u);
  game().drawUnitCircle(*u);
  glPopMatrix();
  game().setCurrentMoveIndex(game().currentMoveIndex() + 1);
  if (game().currentMoveIndex() == game().lastMoveIndex()) {
    endMovement(toI);
  }
}

void EventMoveVisualizer::getCurrentMovingNodes(V2i* from, V2i* to) {
  int i = game().currentMoveIndex();
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

void EventMoveVisualizer::endMovement(const V2i& pos) {
  Core& core = game().core();
  Unit* u = core.id2unit(mEventMove.unitID);
  game().setUiMode(UIMode::NORMAL);
  u->pos = pos;
  if (core.selectedUnit()) {
    core.pathfinder().fillMap(*u);
    game().setVaWalkableMap(game().buildWalkableArray());
    core.calculateFow();
    game().setVaFogOfWar(game().buildFowArray());
  }
  core.applyEvent(core.currentEvent());
  core.setCurrentEvent(nullptr);
  if (u->playerID == game().core().currentPlayer().id) {
    if (core.selectedUnit()) {
      core.pathfinder().fillMap(*core.selectedUnit());
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
    if (current > game().currentMoveIndex()) {
      break;
    }
    last = current;
  }
  return game().currentMoveIndex() - last;
}
