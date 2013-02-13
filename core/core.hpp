// See LICENSE file for copyright and license details.

#ifndef CORE__CORE_HPP
#define CORE__CORE_HPP

#include <list>
#include "json/json.h"
#include "core/misc.hpp"
#include "core/v2i.hpp"
#include "core/math.hpp"
#include "core/dir.hpp"
#include "core/los.hpp"
#include "core/unitType.hpp"
#include "core/event.hpp"
#include "core/pathfinder.hpp"
#include "core/map.hpp"
#include "core/unit.hpp"
#include "core/player.hpp"
#include "core/eventManager.hpp"

class Core {
public:
  Core();
  ~Core();

  const std::list<Player*>& players();
  const Player& currentPlayer() const;
  Player& currentPlayer();
  Unit& selectedUnit();
  bool isAnyUnitSelected();
  std::list<Unit*>& units();
  std::list<Unit*>& deadUnits();
  Pathfinder& pathfinder();
  const Map& map() const;
  Map& map();
  EventManager& eventManager();

  void setSelectedUnit(Unit& unit);
  void setCurrentPlayer(Player* player);

  void calculateFow();
  bool isLosClear(const V2i& from, const V2i& to);

  Unit& unitAt(const V2i& pos);
  bool isUnitAt(const V2i& pos);
  Unit& id2unit(int id);
  void addUnit(const V2i& p, int playerID);
  void refreshUnits(int playerID);

private:
  Json::Value mConfig;
  std::list<Player*> mPlayers;
  Player* mCurrentPlayer;
  Unit* mSelectedUnit;
  std::list<Unit*> mUnits;
  std::list<Unit*> mDeadUnits;
  Pathfinder mPathfinder;
  Map mMap;
  EventManager mEventManager;
  int mInitialUnitsPerPlayerCount;
  int mPlayersCount;

  int getNewUnitID();
  void createLocalHuman(int id);
  void initLocalPlayers(std::vector<int> unitIDs);
  void initUnits();
  void initPlayers();
  void initObstacles();
  void cleanFow();
};

#endif
