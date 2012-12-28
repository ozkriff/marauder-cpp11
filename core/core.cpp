// See LICENSE file for copyright and license details.

#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cassert>
#include <ctime>
#include "core/core.h"

Core::Core()
  : pathfinder(*this)
{
}

Core::~Core() {
}

void Core::createLocalHuman(int id) {
  auto p = new Player;
  players.push_back(p);
  p->id = id;
  p->lastEventID = HAVE_NOT_SEEN_ANY_EVENTS;
}

void Core::initLocalPlayers(int n, int* ids) {
  for (int i = 0; i < n; i++) {
    createLocalHuman(ids[i]);
  }
  currentPlayer = players.back();
}

bool Core::inboard(const V2i& p) const {
  return p.x() >= 0 && p.y() >= 0
      && p.x() < MAP_X && p.y() < MAP_Y;
}

Tile& Core::tile(const V2i &p) {
  assert(inboard(p));
  return map[p.y()][p.x()];
}

void Core::incV2i(V2i* pos) const {
  assert(pos);
  assert(inboard(*pos));
  pos->setX(pos->x() + 1);
  if (pos->x() == MAP_X) {
    pos->setX(0);
    pos->setY(pos->y() + 1);
  }
}

int Core::getNewEventId() {
  if (!events.empty()) {
    return events.back()->id + 1;
  } else {
    return 0;
  }
}

bool Core::isLosClear(const V2i& from, const V2i& to) {
  Los los(from, to);
  for (V2i p = los.getNext(); !los.isFinished(); p = los.getNext()) {
    if (unitAt(p) || tile(p).obstacle) {
      return false;
    }
  }
  return true;
}

#define FOR_EACH_TILE(p) \
  for (*p = V2i(0, 0); inboard(*p); incV2i(p))

void Core::cleanFow() {
  V2i p;
  FOR_EACH_TILE(&p) {
    tile(p).fow = 0;
  }
}

void Core::calculateFow() {
  V2i p;
  assert(currentPlayer);
  cleanFow();
  FOR_EACH_TILE(&p) {
    for (auto u : units) {
      int maxDist = getUnitType(u->typeID).rangeOfVision;
      bool isPlayerOk = (u->playerID == currentPlayer->id);
      bool isDistanceOk = (p.distance(u->pos) < maxDist);
      bool isLosOk = isLosClear(p, u->pos);
      if (isPlayerOk && isDistanceOk && isLosOk) {
        tile(p).fow++;
      }
    }
  }
}

Unit* Core::unitAt(const V2i& pos) {
  for (auto u : units) {
    if (u->pos == pos) {
      return u;
    }
  }
  return nullptr;
}

Unit* Core::id2unit(int id) {
  for (auto u : units) {
    if (u->id == id) {
      return u;
    }
  }
  return nullptr;
}

int Core::getNewUnitID() {
  if (units.size() > 0) {
    auto lastUnit = units.back();
    return lastUnit->id + 1;
  } else {
    return 0;
  }
}

void Core::addUnit(const V2i& p, int playerID) {
  auto u = new Unit;
  assert(inboard(p));
  assert(playerID >= 0 && playerID < 16);
  u->id = getNewUnitID();
  u->pos = p;
  u->playerID = playerID;
  u->dir = static_cast<Dir>(rnd(0, 7));
  u->typeID = rnd(0, (int)UnitTypeID::COUNT - 1);
  units.push_back(u);
  calculateFow();
#if 0
  buildFowArray(&vaFogOfWar);
#endif
}

void Core::killUnit(Unit* u) {
  // deleteNode(&units, data2node(units, u));
  units.remove(u);
  delete u;
  if (selectedUnit) {
    pathfinder.fillMap(*selectedUnit);
    calculateFow();
#if 0
    buildWalkableArray(&vaWalkableMap);
    buildFowArray(&vaFogOfWar);
#endif
  }
}

void Core::shoot(Unit *shooter, Unit *target) {
  if (isLosClear(shooter->pos, target->pos)) {
    killUnit(target);
  }
}

void Core::initUnits() {
  selectedUnit = nullptr;
  for (int i = 0; i < 8; i++) {
    V2i p = V2i(rnd(0, MAP_X - 1), rnd(0, MAP_Y - 1));
    if (!tile(p).obstacle && !unitAt(p)) {
      addUnit(p, rnd(0, 1));
    } else {
      i--;
    }
  }
}

void Core::initObstacles() {
  V2i p;
  FOR_EACH_TILE(&p) {
    tile(p).obstacle = ((rand() % 100) > 85);
  }
}

void Core::initPlayers() {
  int id[] = {0, 1};
  initLocalPlayers(2, id);
}

void Core::initLogic() {
  srand(time(nullptr));
  initUnitTypes();
  pathfinder.cleanMap();
  cleanFow();
  initPlayers();
  initObstacles();
  initUnits();
  calculateFow();
}
