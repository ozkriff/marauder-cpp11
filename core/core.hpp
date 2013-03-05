// See LICENSE file for copyright and license details.

#ifndef CORE__CORE_HPP
#define CORE__CORE_HPP

#include <list>
#include "json/json.h"
#include "core/unitType.hpp"
#include "core/pathfinder.hpp"
#include "core/map.hpp"
#include "core/unit.hpp"
#include "core/eventManager.hpp"

class Player;
class Command;
class CommandAttack;
class CommandEndTurn;
class CommandMove;
class EventView;

class Core {
public:
  Core();
  ~Core();

  const std::list<Player*>& players() const;
  const Player& currentPlayer() const;
  Player& currentPlayer();
  Unit& selectedUnit();
  bool isAnyUnitSelected() const;
  std::list<Unit*>& units();
  std::list<Unit*>& deadUnits();
  Pathfinder& pathfinder();
  const Pathfinder& pathfinder() const;
  const Map& map() const;
  Map& map();
  EventManager& eventManager();
  const EventManager& eventManager() const;

  void doCommand(const Command& cmd);

  void setSelectedUnit(Unit& unit);
  void deselectedAnyUnits();
  void setCurrentPlayer(Player* player);

  void calculateFow();
  void cleanFow();
  bool isLosClear(const V2i& from, const V2i& to);

  Unit& unitAt(const V2i& pos);
  bool isUnitAt(const V2i& pos) const;
  Unit& id2unit(int id);
  void addUnit(
      const V2i& p,
      int playerID,
      const UnitType& unitType,
      const Dir& dir);
  void refreshUnits(int playerID);

  const UnitType& unitType(const std::string& name) const;

private:
  Json::Value mConfig;
  std::map<std::string, UnitType> mUnitTypes;
  std::list<Player*> mPlayers;
  Player* mCurrentPlayer;
  Unit* mSelectedUnit;
  std::list<Unit*> mUnits;
  std::list<Unit*> mDeadUnits;
  Pathfinder mPathfinder;
  Map mMap;
  EventManager mEventManager;

  void command(const CommandAttack& cmd);
  void command(const CommandMove& cmd);
  void command(const CommandEndTurn& cmd);

  void initUnitTypes();
  UnitType parseUnitTypeInfo(const Json::Value& unitTypeInfo) const;
  int getNewUnitID() const;
  void createLocalHumanPlayer(int id);
  void loadScenario();
};

#endif
