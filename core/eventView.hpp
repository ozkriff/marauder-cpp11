// See LICENSE file for copyright and license details.

#ifndef CORE__EVENT_VIEW_HPP
#define CORE__EVENT_VIEW_HPP

#include <list>
#include <vector>
#include "core/v2i.hpp"
#include "core/dir.hpp"

class EventAttack;
class EventMove;
class EventEndTurn;

// TODO: implement all
enum class EventViewType {
  ShowObject,
  HideObject,
  Attack,
  EndTurn,
  BeginTurn,
  Move
};

class EventView {
public:
  EventView(EventViewType type);
  virtual ~EventView();

  EventViewType type() const;

private:
  EventViewType mType;
};

class EventShowObjectView : public EventView {
public:
  EventShowObjectView();
  virtual ~EventShowObjectView();
};

class EventHideObjectView : public EventView {
public:
  EventHideObjectView();
  virtual ~EventHideObjectView();
};

class EventAttackView : public EventView {
public:
  EventAttackView(const EventAttack& event);
  virtual ~EventAttackView();

  int victimID() const;
  int attackerID() const;

  int mAttackerID;
  int mVictimID;
};

class EventEndTurnView : public EventView {
public:
  EventEndTurnView(const EventEndTurn& event);
  virtual ~EventEndTurnView();

  int mOldID;
  int mNewID;
};

class EventBeginTurnView : public EventView {
public:
  EventBeginTurnView();
  virtual ~EventBeginTurnView();
};

class EventMoveView : public EventView {
public:
  EventMoveView(const EventMove& event);
  virtual ~EventMoveView();

  int unitID() const;
  const std::vector<V2i>& path() const;

  int mUnitID;
  std::vector<V2i> mPath;
};

#endif
