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

V2i Core::incV2i(const V2i& pos) const {
  assert(inboard(pos));
  V2i newPos = pos;
  newPos.setX(pos.x() + 1);
  if (newPos.x() == MAP_X) {
    newPos.setX(0);
    newPos.setY(newPos.y() + 1);
  }
  return newPos;
}

int Core::getNewEventId() {
  if (!events.empty()) {
    return events.back()->id + 1;
  } else {
    return 0;
  }
}

// Undo all events that this player have not seen yet
void Core::undoUnshownEvents() {
  if (events.size() == 0) {
    return;
  }
  auto i = events.end();
  --i;
  while (i != events.begin()) {
    auto event = *i;
    if (event->id == currentPlayer->lastEventID) {
      break;
    }
    undoEvent(*event);
    --i;
  }
}

void Core::applyEvent(const Event& e) {
  currentPlayer->lastEventID = e.id;
  switch (e.t) {
  case EventTypeID::MOVE:
    applyEventMove(*this, e.e.move);
    break;
  case EventTypeID::END_TURN:
    applyEventEndTurn(*this, e.e.endTurn);
    break;
  default:
    die("event: applyEvent(): "
        "unknown event '%d'\n", e.t);
    break;
  }
  // updateUnitsVisibility();
}


bool Core::isEventVisible(const Event& e) const {
  switch (e.t) {
  case EventTypeID::MOVE:
    return isVisibleEventMove(*this, e.e.move);
  case EventTypeID::END_TURN:
    return true;
  default:
    die("event: isEventVisible(): "
        "unknown event '%d'\n", e.t);
    return true;
  }
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
    e = getNext(events, e);
    assert(e);
  }
}

// TODO: Called before getNextEvent
bool Core::unshownEventsLeft() {
  applyInvisibleEvents();
  if (events.size() == 0) {
    return false;
  } else {
    auto e = events.back();
    return e->id != currentPlayer->lastEventID;
  }
}

// Always called after applyInvisibleEvents
Event* Core::getNextEvent() {
  int id = currentPlayer->lastEventID; // shortcut
  assert(events.size() > 0);
  if (id == HAVE_NOT_SEEN_ANY_EVENTS) {
    return events.front();
  }
  for (auto e : events) {
    if (e->id == id) {
      return getNext(events, e);
    }
  }
  return nullptr;
}

void Core::addEvent(Event* e) {
  assert(e);
  e->id = getNewEventId();
  events.push_back(e);
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
    if (unitAt(p) || tile(p).obstacle) {
      return false;
    }
  }
  return true;
}

#define FOR_EACH_TILE(p) \
  for (p = V2i(0, 0); inboard(p); p = incV2i(p))

void Core::cleanFow() {
  V2i p;
  FOR_EACH_TILE(p) {
    tile(p).fow = 0;
  }
}

void Core::calculateFow() {
  assert(currentPlayer);
  cleanFow();
  V2i p;
  FOR_EACH_TILE(p) {
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
  FOR_EACH_TILE(p) {
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

void Core::undoEvent(const Event& e) {
  switch (e.t) {
  case EventTypeID::MOVE:
    undoEventMove(*this, e.e.move);
    break;
  case EventTypeID::END_TURN:
    // empty
    // TODO: die()?
    break;
  default:
    die("event: undoEvent(): "
        "unknown event type '%d'\n", e.t);
    break;
  }
  // updateUnitsVisibility();
}

// TODO: rename.
Event* Core::getNextEventNode() {
  // Node *node;
  int id = currentPlayer->lastEventID; // shortcut
  if (events.size() == 0) {
    return nullptr;
  }
  if (id == HAVE_NOT_SEEN_ANY_EVENTS) {
    return events.front();
  }
  // find last seen event
  Event *e = nullptr;
  for (auto e : events) {
    if (e->id == id) {
      break;
    }
  }
  if (!e) {
    return nullptr;
  } else {
    return getNext(events, e);
  }
}

void Core::event2log(const Event& e) {
  UNUSED(e);
  // TODO
}

void Core::sendEvent(const Event& e) {
  UNUSED(e);
  // TODO
}
