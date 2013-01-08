// See LICENSE file for copyright and license details.

#ifndef CORE_CORE_H
#define CORE_CORE_H

#include <list>
#include "core/misc.hpp"
#include "core/v2i.hpp"
#include "core/math.hpp"
#include "core/dir.hpp"
#include "core/los.hpp"
#include "core/unit_type.hpp"
#include "core/event.hpp"
#include "core/event/move.hpp"
#include "core/event/end_turn.hpp"
#include "core/path.hpp"
#include "core/map.hpp"
#include "core/unit.hpp"
#include "core/player.hpp"

class Core {
public:
  Core();
  ~Core();

  const std::list<Player*>& players();
  Event& currentEvent();
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
  int getNewEventID();
  void refreshUnits(int playerID);

  void addEvent(Event* e);
  Event* getNextEvent();
  bool unshownEventsLeft();
  void applyInvisibleEvents();
  bool isEventVisible(const Event& e) const;
  void applyEvent(Event& e);
  void undoUnshownEvents();

private:
  std::list<Player*> mPlayers;
  Event* mCurrentEvent;
  Player* mCurrentPlayer;
  Unit* mSelectedUnit;
  std::list<Unit*> mUnits;
  Pathfinder mPathfinder;
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
  void undoEvent(Event& e);
  Event* getNextEventNode();
  void event2log(const Event& e);
  void sendEvent(const Event& e);
};

#endif