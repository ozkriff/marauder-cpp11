// See LICENSE file for copyright and license details.

#include <cstdlib>
#include <cassert>
#include <ctime>
#include <stdexcept>
#include "core/core.hpp"
#include "core/jsonHelpers.hpp"

Core::Core()
  : mConfig(parseConfig("confCore.json")),
    mCurrentPlayer(nullptr),
    mSelectedUnit(nullptr),
    mPathfinder(*this),
    mMap(V2i(0, 0)),
    mEventManager(*this)
{
  srand(std::time(nullptr));
  initUnitTypes();
  loadScenario();
}

Core::~Core() {
  for (auto* unitPointer : mUnits) {
    delete unitPointer;
  }
  for (auto* unitPointer : mDeadUnits) {
    delete unitPointer;
  }
  for (auto* playerPointer : mPlayers) {
    delete playerPointer;
  }
}

const std::list<Player*>& Core::players() const {
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

bool Core::isAnyUnitSelected() const {
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

const Pathfinder& Core::pathfinder() const {
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

const EventManager& Core::eventManager() const {
  return mEventManager;
}

void Core::setSelectedUnit(Unit& unit) {
  mSelectedUnit = &unit;
}

void Core::deselectedAnyUnits() {
  mSelectedUnit = nullptr;
}

void Core::setCurrentPlayer(Player* player) {
  mCurrentPlayer = player;
}

void Core::createLocalHuman(int id) {
  auto* p = new Player;
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
  for (auto* u : units()) {
    if (u->playerID() == playerID) {
      u->setActionPoints(u->type().actionPoints);
    }
  }
}

const UnitType& Core::getUnitType(const std::string& name) const {
  assert(mUnitTypes.count(name) != 0);
  return mUnitTypes.at(name);
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
  map().forEachTile([](Tile& tile) {
    tile.fow = 0;
  });
}

void Core::calculateFow() {
  assert(mCurrentPlayer);
  map().forEachPos([this](const V2i& p) {
    for (auto* u : mUnits) {
      int maxDist = u->type().rangeOfVision;
      bool isPlayerOk = (u->playerID() == mCurrentPlayer->id);
      bool isDistanceOk = (p.distance(u->position()) < maxDist);
      bool isLosOk = isLosClear(p, u->position());
      if (isPlayerOk && isDistanceOk && isLosOk) {
        map().tile(p).fow++;
      }
    }
  });
}

Unit& Core::unitAt(const V2i& pos) {
  for (auto* u : mUnits) {
    if (u->position() == pos) {
      return *u;
    }
  }
  throw std::logic_error("No unit at pos!");
}

bool Core::isUnitAt(const V2i& pos) const {
#if 0
  return (map().tile(pos).unit != NULL);
#else
  for (auto* u : mUnits) {
    if (u->position() == pos) {
      return true;
    }
  }
  return false;
#endif
}

Unit& Core::id2unit(int id) {
  for (auto* u : mUnits) {
    if (u->id() == id) {
      return *u;
    }
  }
  throw std::logic_error("No unit with this ID!");
}

void Core::initUnitTypes() {
  Json::Value config = parseConfig("unitTypes.json");
  for (const std::string& unitTypeName : config.getMemberNames()) {
    UnitType unitType = parseUnitTypeInfo(config[unitTypeName]);
    mUnitTypes[unitTypeName] = unitType;
  }
}

UnitType Core::parseUnitTypeInfo(const Json::Value& unitTypeInfo) const {
  UnitType unitType;
  unitType.rangeOfVision = unitTypeInfo["rangeOfVision"].asInt();
  unitType.actionPoints = unitTypeInfo["actionPoints"].asInt();
  unitType.id = mUnitTypes.size();
  return unitType;
}

int Core::getNewUnitID() const {
  if (!mUnits.empty()) {
    auto* lastUnit = mUnits.back();
    return lastUnit->id() + 1;
  } else {
    return 0;
  }
}

void Core::addUnit(
    const V2i& p,
    int playerID,
    const UnitType& unitType,
    const Dir& dir)
{
  assert(map().isInboard(p));
  auto* u = new Unit(getNewUnitID(), playerID, unitType);
  u->setPosition(p);
  u->setDirection(dir);
  u->setActionPoints(u->type().actionPoints);
  mUnits.push_back(u);
  calculateFow();
}

void Core::loadScenario() {
  Json::Value scenario = parseConfig("scenario.json");
  // players
  {
    int playersCount = scenario["playersCount"].asInt();
    std::vector<int> playerIDs;
    for (int i = 0; i < playersCount; i++) {
      playerIDs.push_back(i);
    }
    initLocalPlayers(playerIDs);
  }
  // map
  {
    mMap = Map(JsonValueToV2i(scenario["mapSize"]));
  }
  // obstacles
  {
    Json::Value mapData = scenario["map"];
    int y = 0;
    for (const Json::Value& row : mapData) {
      int x = 0;
      for (const Json::Value& tile : row) {
        V2i p(x, y);
        bool isObstacle = (tile.asString() == "obstacle");
        map().tile(p).obstacle = isObstacle;
        x++;
      }
      y++;
    }
  }
  // units
  {
    Json::Value unitsData = scenario["units"];
    int playerID = 0;
    for (const Json::Value& playerUnitData : unitsData) {
      for (const Json::Value& unitInfo : playerUnitData) {
        V2i position = JsonValueToV2i(unitInfo["position"]);
        std::string typeName = unitInfo["type"].asString();
        Dir direction(unitInfo["direction"].asInt());
        addUnit(position, playerID, getUnitType(typeName), direction);
      }
      playerID++;
    }
    calculateFow();
  }
}
