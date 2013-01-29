// See LICENSE file for copyright and license details.

#include <cstdlib>
#include <cassert>
#include <ctime>
#include <stdexcept>
#include "core/core.hpp"

Core::Core()
  : mCurrentEvent(nullptr),
    mCurrentPlayer(nullptr),
    mSelectedUnit(nullptr),
    mPathfinder(*this),
    mMap(V2i(20, 18))
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

Event& Core::currentEvent() {
  return *mCurrentEvent;
}

const Player& Core::currentPlayer() {
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

void Core::setSelectedUnit(Unit& unit) {
  mSelectedUnit = &unit;
}

void Core::setCurrentEvent(Event* event) {
  mCurrentEvent = event;
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

bool Core::isInboard(const V2i& p) const {
  return map().isInboard(p);
}

Tile& Core::tile(const V2i& p) {
  return map().tile(p);
}

int Core::getNewEventID() {
  if (!mEvents.empty()) {
    return mEvents.back()->id() + 1;
  } else {
    return 0;
  }
}

void Core::refreshUnits(int playerID) {
  for (auto u : units()) {
    if (u->playerID == playerID) {
      u->actionPoints = getUnitType(u->typeID).actionPoints;
    }
  }
}

// Undo all events that this player have not seen yet
void Core::undoUnshownEvents() {
  if (mEvents.empty()) {
    return;
  }
  auto i = mEvents.end();
  --i;
  while (i != mEvents.begin()) {
    auto event = *i;
    if (event->id() == mCurrentPlayer->lastSeenEventID) {
      break;
    }
    undoEvent(*event);
    --i;
  }
}

void Core::applyEvent(Event& e) {
  mCurrentPlayer->lastSeenEventID = e.id();
  e.apply(*this);
}

bool Core::isEventVisible(const Event& e) const {
  return e.isVisible(*this);
}

// TODO simplify
void Core::applyInvisibleEvents() {
  Event* e = getNextEventNode();
  while (e) {
    assert(e);
    if (!isEventVisible(*e)) {
      applyEvent(*e);
    } else {
      break;
    }
    e = getNext(mEvents, e);
    assert(e);
  }
}

// TODO: Called before getNextEvent
bool Core::unshownEventsLeft() {
  applyInvisibleEvents();
  if (mEvents.empty()) {
    return false;
  } else {
    auto e = mEvents.back();
    return e->id() != mCurrentPlayer->lastSeenEventID;
  }
}

// Always called after applyInvisibleEvents
Event* Core::getNextEvent() {
  int id = mCurrentPlayer->lastSeenEventID; // shortcut
  assert(!mEvents.empty());
  if (id == HAVE_NOT_SEEN_ANY_EVENTS) {
    return mEvents.front();
  }
  for (auto e : mEvents) {
    if (e->id() == id) {
      return getNext(mEvents, e);
    }
  }
  return nullptr;
}

void Core::addEvent(Event* e) {
  assert(e);
  mEvents.push_back(e);
  // event2log(*e);
#if 0
  // TODO
  if (!isLocal) {
    sendEvent(e);
  }
#else
  // UNUSED(sendEvent);
#endif
}

bool Core::isLosClear(const V2i& from, const V2i& to) {
  Los los(from, to);
  for (V2i p = los.getNext(); !los.isFinished(); p = los.getNext()) {
#if 1
    // TODO: temp hack. fix los, remove this.
    if (!isInboard(p)) {
      return false;
    }
#endif
    if (isUnitAt(p) || tile(p).obstacle) {
      return false;
    }
  }
  return true;
}

void Core::cleanFow() {
  V2i p;
  FOR_EACH_TILE(map(), p) {
    tile(p).fow = 0;
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
        tile(p).fow++;
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

Unit* Core::id2unit(int id) {
  for (auto u : mUnits) {
    if (u->id == id) {
      return u;
    }
  }
  return nullptr;
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
  assert(isInboard(p));
  assert(playerID >= 0 && playerID < 16);
  u->id = getNewUnitID();
  u->pos = p;
  u->playerID = playerID;
  u->dir = static_cast<Dir>(rnd(0, 6 - 1));
  u->typeID = rnd(0, static_cast<int>(UnitTypeID::COUNT) - 1);
  u->actionPoints = getUnitType(u->typeID).actionPoints;
  mUnits.push_back(u);
  calculateFow();
#if 0
  buildFowArray(&vaFogOfWar);
#endif
}

void Core::killUnit(Unit* u) {
  mUnits.remove(u);
  delete u;
  if (mSelectedUnit) {
    mPathfinder.fillMap(*mSelectedUnit);
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
  for (int i = 0; i < 8; i++) {
    V2i p(rnd(0, map().size().x() - 1), rnd(0, map().size().y() - 1));
    if (!tile(p).obstacle && !isUnitAt(p)) {
      addUnit(p, rnd(0, 1));
    } else {
      i--;
    }
  }
}

void Core::initObstacles() {
  V2i p;
  FOR_EACH_TILE(map(), p) {
    tile(p).obstacle = ((rand() % 100) > 85);
  }
}

void Core::initPlayers() {
  initLocalPlayers({0, 1});
}

void Core::undoEvent(Event& e) {
  e.undo(*this);
}

// TODO: rename.
Event* Core::getNextEventNode() {
  int id = mCurrentPlayer->lastSeenEventID; // shortcut
  if (mEvents.empty()) {
    return nullptr;
  }
  if (id == HAVE_NOT_SEEN_ANY_EVENTS) {
    return mEvents.front();
  }
  // find last seen event
  for (auto e : mEvents) {
    if (e->id() == id) {
      return getNext(mEvents, e);
    }
  }
  return nullptr; // if there is no event with that id
}

void Core::event2log(const Event& e) {
  UNUSED(e);
  // TODO
}

void Core::sendEvent(const Event& e) {
  UNUSED(e);
  // TODO
}
