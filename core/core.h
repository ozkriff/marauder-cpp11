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

struct Unit {
  int typeID;
  int id;
  int playerID;
  Dir dir;
  int actionPoints;
  V2i pos;
};

struct Player {
  int id;
  int lastEventID;
};

struct Tile {
  Unit* unit;
  bool obstacle;
  // TODO: move to pathfinding
  int cost;
  Dir parent;
  Dir dir;
  int fow; // how many units see this tile
};

#define MAP_X 20
#define MAP_Y 20

// Player.last_event_id
#define HAVE_NOT_SEEN_ANY_EVENTS (-1)

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
  
  void setSelectedUnit(Unit* unit);
  void setCurrentEvent(Event* event);
  void setCurrentPlayer(Player* player);

  Tile& tile(const V2i &p);
  void calculateFow();
  Unit* unitAt(const V2i& pos);
  Unit* id2unit(int id);
  void addUnit(const V2i& p, int playerID);
  void shoot(Unit* shooter, Unit* target);
  void killUnit(Unit* u);
  void initLogic();
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
  Tile mMap[MAP_Y][MAP_X];
  std::list<Event*> mEvents;

  void createLocalHuman(int id);
  void initLocalPlayers(int n, int* ids);
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
