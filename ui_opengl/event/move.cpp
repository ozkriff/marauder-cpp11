// See LICENSE file for copyright and license details.

#include "ui_opengl/event/move.hpp"
#include <cassert>
#include <SDL/SDL_opengl.h>
#include "core/misc.hpp"
#include "core/v2i.hpp"
#include "core/dir.hpp"
#include "core/core.hpp"
#include "core/path.hpp"
#include "ui_opengl/v2f.hpp"
#include "ui_opengl/vertex_array.hpp"
#include "ui_opengl/game.hpp"

int getLastEventMoveIndex(Game& game, const Event &e) {
  UNUSED(game);
  auto eventMove = dynamic_cast<const EventMove*>(&e);
  return (eventMove->path.size() - 1) * moveSpeed;
}

void getCurrentMovingNodes(
    Game& game, const EventMove& e, V2i* from, V2i* to)
{
  int i = game.currentMoveIndex();
  auto& p = e.path; // shortcut
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

static void endMovement(Game& game, const EventMove& e, const V2i& pos) {
  Unit* u = game.core().id2unit(e.unitID);
  game.setUiMode(UIMode::NORMAL);
  u->pos = pos;
  if (game.core().selectedUnit()) {
    game.core().pathfinder().fillMap(*u);
    game.setVaWalkableMap(game.buildWalkableArray());
    game.core().calculateFow();
    game.setVaFogOfWar(game.buildFowArray());
  }
  game.core().applyEvent(game.core().currentEvent());
  game.core().setCurrentEvent(nullptr);
  if (u->playerID == game.core().currentPlayer().id) {
    if (game.core().selectedUnit()) {
      game.core().pathfinder().fillMap(*game.core().selectedUnit());
      game.setVaWalkableMap(game.buildWalkableArray());
    }
    game.setVaFogOfWar(game.buildFowArray());
  }
}

static int getNodeIndex(Game& game, const EventMove& e) {
  int last = 0;
  int current = 0;
  for (unsigned int j = 0; j < e.path.size() - 2; j++) {
    current += moveSpeed;
    if (current > game.currentMoveIndex()) {
      break;
    }
    last = current;
  }
  return game.currentMoveIndex() - last;
}

void drawMovingUnit(Game& game, const EventMove& e) {
  Unit* u = game.core().id2unit(e.unitID);
  V2i fromI, toI;
  getCurrentMovingNodes(game, e, &fromI, &toI);
  V2f fromF = game.v2iToV2f(fromI);
  V2f toF = game.v2iToV2f(toI);
  int nodeIndex = getNodeIndex(game, e);
  V2f diff = (toF - fromF) / moveSpeed;
  V2f p = fromF + diff * nodeIndex;
  glPushMatrix();
  glTranslatef(p.x(), p.y(), 0.0f);
  // TODO: Remove '+ 4'! Rotate obj files!
  glRotatef(
      Dir(fromI, toI).toInt() * 60.0f + 120.0f,
      0, 0, 1);
  game.drawUnitModel(*u);
  game.drawUnitCircle(*u);
  glPopMatrix();
  game.setCurrentMoveIndex(game.currentMoveIndex() + 1);
  if (game.currentMoveIndex() == game.lastMoveIndex()) {
    endMovement(game, e, toI);
  }
}
