// See LICENSE file for copyright and license details.

#ifndef CORE_CORE_H
#define CORE_CORE_H

#include <list>
#include "core/misc.h"
#include "core/v2i.h"
#include "core/math.h"
#include "core/dir.h"
#include "core/los.h"
#include "core/unit_type.h"
#include "core/event.h"
#include "core/path.h"
#include "core/map.h"
#include "core/unit.h"
#include "core/player.h"

class Core {
public:
  Core();
  ~Core();

  const std::list<Player*>& players();
  const Event& currentEvent();
  const Player& currentPlayer();
  Unit* selectedUnit();
  std::list<Unit*>& units();
  Pathfinder& pathfinder();
  const Map& map() const;
  Map& map();

  void setSelectedUnit(Unit* unit);
  void setCurrentEvent(Event* event);
  void setCurrentPlayer(Player* player);

  Tile& tile(const V2i& p);
  void calculateFow();
  Unit* unitAt(const V2i& pos);
  Unit* id2unit(int id);
  void addUnit(const V2i& p, int playerID);
  void shoot(Unit* shooter, Unit* target);
  void killUnit(Unit* u);
  bool inboard(const V2i& p) const;
  V2i incV2i(const V2i& pos) const;
  int getNewEventID();
  void refreshUnits(int playerID);

  void addEvent(Event* e);
  Event* getNextEvent();
  bool unshownEventsLeft();
  void applyInvisibleEvents();
  bool isEventVisible(const Event& e) const;
  void applyEvent(const Event& e);
  void undoUnshownEvents();

private:
  std::list<Player*> mPlayers;
  Event const* mCurrentEvent;
  Player* mCurrentPlayer;
  Unit* mSelectedUnit;
  std::list<Unit*> mUnits;
  Pathfinder mPathfinder;
  // Tile mMap[MAP_Y][MAP_X];
  Map mMap;
  std::list<Event*> mEvents;

  void createLocalHuman(int id);
  void initLocalPlayers(std::vector<int> unitIDs);
  int getNewUnitID();
  void initUnits();
  void initPlayers();
  void initObstacles();
  void cleanFow();
  bool isLosClear(const V2i& from, const V2i& to);
  void undoEvent(const Event& e);
  Event* getNextEventNode();
  void event2log(const Event& e);
  void sendEvent(const Event& e);
};

#endif
