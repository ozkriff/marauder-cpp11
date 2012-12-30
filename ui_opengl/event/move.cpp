// See LICENSE file for copyright and license details.

#include <cassert>
#include <SDL/SDL_opengl.h>
#include "core/misc.h"
#include "core/v2i.h"
#include "core/dir.h"
#include "core/core.h"
#include "core/path.h"
#include "ui_opengl/v2f.h"
#include "ui_opengl/vertex_array.h"
#include "ui_opengl/game.h"

static int getMoveLegth(const V2i& from, const V2i& to) {
  if (Dir(from, to).isDiagonal()) {
    return 14;
  } else {
    return 10;
  }
}

int getLastEventMoveIndex(Game& game, const Event &e) {
  const EventMove* m = &e.e.move;
  auto& p = m->path; // shortcut
  int length = 0;
  int i;
  for (i = 1; i < m->length; i++) {
    length += getMoveLegth(p[i - 1], p[i]);
  }
  return length;
}

void getCurrentMovingNodes(
    Game& game, const EventMove& e, V2i* from, V2i* to)
{
  int i = game.currentMoveIndex;
  auto& p = e.path; // shortcut
  unsigned int j; // node id
  assert(from);
  assert(to);
  for (j = 0; j < p.size() - 2; j++) {
    i -= getMoveLegth(p[j], p[j + 1]);
    if (i < 0) {
      break;
    }
  }
  *from = p[j];
  *to = p[j + 1];
}

static void endMovement(Game& game, const EventMove& e, const V2i *pos) {
  Unit *u;
  assert(pos);
  u = game.core.id2unit(e.unitID);
  game.uiMode = UIMode::NORMAL;
  u->pos = *pos;
  if (game.core.selectedUnit) {
    game.core.pathfinder.fillMap(*u);
    game.vaWalkableMap = game.buildWalkableArray();
    game.core.calculateFow();
    game.vaFogOfWar = game.buildFowArray();
  }
  game.core.applyEvent(*game.core.currentEvent);
  game.core.currentEvent = nullptr;
  if (u->playerID == game.core.currentPlayer->id) {
    if (game.core.selectedUnit) {
      game.core.pathfinder.fillMap(*game.core.selectedUnit);
      game.vaWalkableMap = game.buildWalkableArray();
    }
    game.vaFogOfWar = game.buildFowArray();
  }
}

static int getNodeIndex(Game& game, const EventMove& e) {
  auto& p = e.path; // shortcut
  int last = 0;
  int current = 0;
  for (unsigned int j = 0; j < p.size() - 2; j++) {
    current += getMoveLegth(p[j], p[j + 1]);
    if (current > game.currentMoveIndex) {
      break;
    }
    last = current;
  }
  return game.currentMoveIndex - last;
}

void drawMovingUnit(Game& game, const EventMove& e) {
  Unit *u = game.core.id2unit(e.unitID);
  V2i fromI, toI;
  V2f diff;
  V2f p;
  getCurrentMovingNodes(game, e, &fromI, &toI);
  V2f fromF = game.v2iToV2f(fromI);
  V2f toF = game.v2iToV2f(toI);
  int moveSpeed = getMoveLegth(fromI, toI);
  int nodeIndex = getNodeIndex(game, e);
  diff.setX((toF.x() - fromF.x()) / moveSpeed);
  diff.setY((toF.y() - fromF.y()) / moveSpeed);
  p.setX(fromF.x() + diff.x() * nodeIndex);
  p.setY(fromF.y() + diff.y() * nodeIndex);
  glPushMatrix();
  glTranslatef(p.x(), p.y(), 0.0f);
  // TODO: Remove '+ 4'! Rotate obj files!
  glRotatef(
      (Dir(fromI, toI).toInt() + 4) * 45.0f,
      0, 0, 1);
  game.drawUnitModel(*u);
  game.drawUnitCircle(*u);
  glPopMatrix();
  game.currentMoveIndex++;
  if (game.currentMoveIndex == game.lastMoveIndex) {
    endMovement(game, e, &toI);
  }
}
