// See LICENSE file for copyright and license details.

#include "core/eventView.hpp"
#include "core/event/eventMove.hpp"
#include "core/event/eventAttack.hpp"
#include "core/event/eventEndTurn.hpp"

EventView::EventView(EventViewType type)
  : mType(type)
{}

EventView::~EventView() {
}

EventViewType EventView::type() const {
  return mType;
}


EventShowObjectView::EventShowObjectView()
  : EventView(EventViewType::ShowObject)
{}

EventShowObjectView::~EventShowObjectView() {
}


EventHideObjectView::EventHideObjectView()
  : EventView(EventViewType::HideObject)
{}

EventHideObjectView::~EventHideObjectView() {
}


EventAttackView::EventAttackView(const EventAttack& event)
  : EventView(EventViewType::Attack),
    mAttackerID(event.attackerID()),
    mVictimID(event.victimID())
{}

EventAttackView::~EventAttackView() {
}

int EventAttackView::victimID() const {
  return mVictimID;
}

int EventAttackView::attackerID() const {
  return mAttackerID;
}

EventEndTurnView::EventEndTurnView(const EventEndTurn& event)
  : EventView(EventViewType::EndTurn),
    mOldID(event.oldID()),
    mNewID(event.newID())
{}

EventEndTurnView::~EventEndTurnView() {
}


EventBeginTurnView::EventBeginTurnView()
  : EventView(EventViewType::BeginTurn)
{}

EventBeginTurnView::~EventBeginTurnView() {
}


EventMoveView::EventMoveView(const EventMove& event)
  : EventView(EventViewType::Move),
    mUnitID(event.unitID()),
    mPath(event.path())
{}

EventMoveView::~EventMoveView() {
}

int EventMoveView::unitID() const {
  return mUnitID;
}

const std::vector<V2i>& EventMoveView::path() const {
  return mPath;
}
