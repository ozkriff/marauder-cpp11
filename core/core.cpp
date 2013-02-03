// See LICENSE file for copyright and license details.

#include <cstdlib>
#include <cassert>
#include <ctime>
#include <stdexcept>
#include "core/core.hpp"

Core::Core()
  : mCurrentPlayer(nullptr),
    mSelectedUnit(nullptr),
    mPathfinder(*this),
    mMap(V2i(20, 18)),
    mEventManager(*this)
{
  srand(std::time(nullptr));
  initUnitTypes();
  mPathfinder.cleanMap();
  cleanFow();
  initPlayers();
  initObstacles();
  initUnits();
  calculateFow();
}

Core::~Core() {
}

const std::list<Player*>& Core::players() {
  return mPlayers;
}

const Player& Core::currentPlayer() const {
  return *mCurrentPlayer;
}

Player& Core::currentPlayer() {
  return *mCurrentPlayer;
}

Unit& Core::selectedUnit() {
  return *mSelectedUnit;
}

bool Core::isAnyUnitSelected() {
  return mSelectedUnit != NULL;
}

std::list<Unit*>& Core::units() {
  return mUnits;
}

std::list<Unit*>& Core::deadUnits() {
  return mDeadUnits;
}

Pathfinder& Core::pathfinder() {
  return mPathfinder;
}

const Map& Core::map() const {
  return mMap;
}

Map& Core::map() {
  return mMap;
}

EventManager& Core::eventManager() {
  return mEventManager;
}

void Core::setSelectedUnit(Unit& unit) {
  mSelectedUnit = &unit;
}

void Core::setCurrentPlayer(Player* player) {
  mCurrentPlayer = player;
}
  
void Core::createLocalHuman(int id) {
  auto p = new Player;
  mPlayers.push_back(p);
  p->id = id;
  p->lastSeenEventID = HAVE_NOT_SEEN_ANY_EVENTS;
}

void Core::initLocalPlayers(std::vector<int> unitIDs) {
  for (int id : unitIDs) {
    createLocalHuman(id);
  }
  mCurrentPlayer = mPlayers.back();
}

void Core::refreshUnits(int playerID) {
  for (auto u : units()) {
    if (u->playerID == playerID) {
      u->actionPoints = getUnitType(u->typeID).actionPoints;
    }
  }
}

bool Core::isLosClear(const V2i& from, const V2i& to) {
  Los los(from, to);
  for (V2i p = los.getNext(); !los.isFinished(); p = los.getNext()) {
#if 1
    // TODO: temp hack. fix los, remove this.
    if (!map().isInboard(p)) {
      return false;
    }
#endif
    if (isUnitAt(p) || map().tile(p).obstacle) {
      return false;
    }
  }
  return true;
}

void Core::cleanFow() {
  V2i p;
  FOR_EACH_TILE(map(), p) {
    map().tile(p).fow = 0;
  }
}

void Core::calculateFow() {
  assert(mCurrentPlayer);
  cleanFow();
  V2i p;
  FOR_EACH_TILE(map(), p) {
    for (auto u : mUnits) {
      int maxDist = getUnitType(u->typeID).rangeOfVision;
      bool isPlayerOk = (u->playerID == mCurrentPlayer->id);
      bool isDistanceOk = (p.distance(u->pos) < maxDist);
      bool isLosOk = isLosClear(p, u->pos);
      if (isPlayerOk && isDistanceOk && isLosOk) {
        map().tile(p).fow++;
      }
    }
  }
}

Unit& Core::unitAt(const V2i& pos) {
  for (auto u : mUnits) {
    if (u->pos == pos) {
      return *u;
    }
  }
  throw std::logic_error("No unit at pos!");
}

bool Core::isUnitAt(const V2i& pos) {
#if 0
  return (map().tile(pos).unit != NULL);
#else
  for (auto u : mUnits) {
    if (u->pos == pos) {
      return true;
    }
  }
  return false;
#endif
}

Unit& Core::id2unit(int id) {
  for (auto u : mUnits) {
    if (u->id == id) {
      return *u;
    }
  }
  throw std::logic_error("No unit with this ID!");
}

int Core::getNewUnitID() {
  if (!mUnits.empty()) {
    auto lastUnit = mUnits.back();
    return lastUnit->id + 1;
  } else {
    return 0;
  }
}

void Core::addUnit(const V2i& p, int playerID) {
  auto u = new Unit;
  assert(map().isInboard(p));
  assert(playerID >= 0 && playerID < 16);
  u->id = getNewUnitID();
  u->pos = p;
  u->playerID = playerID;
  u->dir = Dir(rnd(0, 6 - 1));
  u->typeID = rnd(0, static_cast<int>(UnitTypeID::COUNT) - 1);
  u->actionPoints = getUnitType(u->typeID).actionPoints;
  mUnits.push_back(u);
  calculateFow();
#if 0
  buildFowArray(&vaFogOfWar);
#endif
}

void Core::initUnits() {
  for (int i = 0; i < 8; i++) {
    V2i p(rnd(0, map().size().x() - 1), rnd(0, map().size().y() - 1));
    if (!map().tile(p).obstacle && !isUnitAt(p)) {
      int playerID = rnd(0, 1);
      addUnit(p, playerID);
    } else {
      i--;
    }
  }
}

void Core::initObstacles() {
  V2i p;
  FOR_EACH_TILE(map(), p) {
    map().tile(p).obstacle = ((rand() % 100) > 85);
  }
}

void Core::initPlayers() {
  initLocalPlayers({0, 1});
}

