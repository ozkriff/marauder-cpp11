// See LICENSE file for copyright and license details.

#include <cstdlib>
#include <cassert>
#include <ctime>
#include <stdexcept>
#include "core/core.hpp"
#include "core/jsonHelpers.hpp"
#include "core/command.hpp"
#include "core/command/commandMove.hpp"
#include "core/command/commandAttack.hpp"
#include "core/command/commandEndTurn.hpp"
#include "core/event/eventMove.hpp"
#include "core/event/eventAttack.hpp"
#include "core/event/eventEndTurn.hpp"
#include "core/player.hpp"
#include "core/los.hpp"
#include "core/eventView.hpp"

Core::Core()
  : mConfig(parseJsonFile("confCore.json")),
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

std::list<EventView*>& Core::eventViewList(PlayerID playerID) {
  return mEventViewLists.at(playerID);
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

void Core::doCommand(const Command& cmd) {
  switch (cmd.type()) {
  case CommandType::Move:
    command(dynamic_cast<const CommandMove&>(cmd));
    break;
  case CommandType::EndTurn:
    command(dynamic_cast<const CommandEndTurn&>(cmd));
    break;
  case CommandType::Attack:
    command(dynamic_cast<const CommandAttack&>(cmd));
    break;
  default:
    throw std::logic_error("default case");
  }
  processNewEvents();
}

void Core::processNewEvents() {
  while (!eventManager().isEmpty()) {
    Event* pointerToEvent = eventManager().popEvent();
    const Event& event = *pointerToEvent; // TODO: Fix memory leak
    event.apply(*this);
    for (Player* pPlayer : players()) {
      const Player& player = *pPlayer;
      EventView* eventView = eventToEventView(event); // TODO: send current player?
      mEventViewLists.at(player.id).push_back(eventView);
    }
  }
}

void Core::command(const CommandAttack& cmd) {
  const Unit& attacker = id2unit(cmd.attackerID());
  const Unit& victim = id2unit(cmd.victimID());
  if (isLosClear(attacker.position(), victim.position())
    && attacker.actionPoints() >= 3)
  {
    Event* e = EventAttack::generate(*this, attacker, victim);
    eventManager().addEvent(e);

  }
}

void Core::command(const CommandMove& cmd) {
  const Unit& unit = id2unit(cmd.unitID());
  const Tile& tile = map().tile(cmd.destination());
  int actionPoints = unit.actionPoints();
  if (tile.cost <= actionPoints && tile.parent.value() != DirID::NONE) {
    if (tile.cost <= actionPoints) {
      Event* e = EventMove::generate(*this, unit, cmd.destination());
      eventManager().addEvent(e);
    }
  }
}

void Core::command(const CommandEndTurn& cmd) {
  UNUSED(cmd);
  Event* e = EventEndTurn::generate(*this);
  eventManager().addEvent(e);
}

void Core::createLocalHumanPlayer(PlayerID id) {
  auto* p = new Player;
  mPlayers.push_back(p);
  p->id = id;
  {
    // create empty list object for EventViews
    mEventViewLists[id] = std::list<EventView*>();
  }
}

void Core::refreshUnits(PlayerID playerID) {
  for (auto* u : units()) {
    if (u->playerID() == playerID) {
      u->setActionPoints(u->type().actionPoints);
    }
  }
}

const UnitType& Core::unitType(const std::string& name) const {
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
    for (const auto* u : mUnits) {
      int maxDist = u->type().rangeOfVision;
      bool isPlayerOk = (u->playerID() == mCurrentPlayer->id);
      bool isDistanceOk = (p.distance(u->position()) < maxDist);
      bool isLosOk = isLosClear(p, u->position());
      if (isPlayerOk && isDistanceOk && isLosOk) {
        ++(map().tile(p).fow);
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
  throw std::invalid_argument("No unit at pos!");
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

Unit& Core::id2unit(UnitID id) {
  for (auto* u : mUnits) {
    if (u->id() == id) {
      return *u;
    }
  }
  throw std::invalid_argument("No unit with this ID!");
}

// TODO: actually _DO_ something!
EventView* Core::eventToEventView(const Event& event) {
  switch (event.type()) {
  case EventType::Move:
    return new EventMoveView(dynamic_cast<const EventMove&>(event));
  case EventType::EndTurn:
    return new EventEndTurnView(dynamic_cast<const EventEndTurn&>(event));
  case EventType::Attack:
    return new EventAttackView(dynamic_cast<const EventAttack&>(event));
  default:
    throw std::logic_error("default case!");
  }
}

void Core::initUnitTypes() {
  Json::Value config = parseJsonFile("unitTypes.json");
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

UnitID Core::getNewUnitID() const {
  if (!mUnits.empty()) {
    auto* lastUnit = mUnits.back();
    return lastUnit->id() + 1;
  } else {
    return 0;
  }
}

void Core::addUnit(
    const V2i& p,
    PlayerID playerID,
    const UnitType& unitType,
    const Dir& dir)
{
  assert(map().isInboard(p));
  auto* u = new Unit(getNewUnitID(), playerID, unitType);
  u->setPosition(p);
  u->setDirection(dir);
  u->setActionPoints(u->type().actionPoints);
  mUnits.push_back(u);
  if (isAnyUnitSelected()) {
    pathfinder().fillMap(selectedUnit());
  }
  calculateFow();
}

void Core::loadScenario() {
  Json::Value scenario = parseJsonFile("scenario.json");
  // players
  {
    int playersCount = scenario["playersCount"].asInt();
    for (PlayerID id = 0; id < playersCount; ++id) {
      createLocalHumanPlayer(id);
    }
    mCurrentPlayer = mPlayers.back(); // Important!
  }
  // map
  {
    Json::Value mapFile = parseJsonFile(scenario["map"].asString());
    mMap = Map(JsonValueToV2i(mapFile["mapSize"]));
    Json::Value RowsOfTiles = mapFile["tiles"];
    int y = 0;
    for (const Json::Value& rowOfTiles : RowsOfTiles) {
      int x = 0;
      for (const Json::Value& tile : rowOfTiles) {
        V2i p(x, y);
        bool isObstacle = (tile.asString() == "obstacle");
        map().tile(p).obstacle = isObstacle;
        ++x;
      }
      ++y;
    }
  }
  // units
  {
    Json::Value unitsData = scenario["units"];
    PlayerID playerID = 0;
    for (const Json::Value& playerUnitData : unitsData) {
      for (const Json::Value& unitInfo : playerUnitData) {
        V2i position = JsonValueToV2i(unitInfo["position"]);
        std::string typeName = unitInfo["type"].asString();
        Dir direction(unitInfo["direction"].asInt());
        addUnit(position, playerID, unitType(typeName), direction);
      }
      ++playerID;
    }
    calculateFow();
  }
}
