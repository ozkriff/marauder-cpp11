// See LICENSE file for copyright and license details.

#ifndef CORE__CORE_HPP
#define CORE__CORE_HPP

#include <list>
#include "core/misc.hpp"
#include "core/v2i.hpp"
#include "core/math.hpp"
#include "core/dir.hpp"
#include "core/los.hpp"
#include "core/unitType.hpp"
#include "core/event.hpp"
#include "core/event/eventMove.hpp"
#include "core/event/eventEndTurn.hpp"
#include "core/pathfinder.hpp"
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
  Unit& selectedUnit();
  bool isAnyUnitSelected();
  std::list<Unit*>& units();
  std::list<Unit*>& deadUnits();
  Pathfinder& pathfinder();
  const Map& map() const;
  Map& map();

  void setSelectedUnit(Unit& unit);
  void setCurrentEvent(Event* event);
  void setCurrentPlayer(Player* player);

  Tile& tile(const V2i& p);
  void calculateFow();
  Unit& unitAt(const V2i& pos);
  bool isUnitAt(const V2i& pos);
  Unit* id2unit(int id);
  void addUnit(const V2i& p, int playerID);
  void shoot(Unit* shooter, Unit* target);
  void killUnit(Unit* u);
  bool isInboard(const V2i& p) const;
  int getNewEventID();
  void refreshUnits(int playerID);
  bool isLosClear(const V2i& from, const V2i& to);

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
  std::list<Unit*> mDeadUnits;
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
  void undoEvent(Event& e);
  Event* getNextEventNode();
  void event2log(const Event& e);
  void sendEvent(const Event& e);
};

#endif
