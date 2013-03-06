// See LICENSE file for copyright and license details.

#ifndef CORE__EVENT__EVENT_END_TURN_HPP
#define CORE__EVENT__EVENT_END_TURN_HPP

#include "core/event.hpp"

class EventEndTurn : public Event {
public:
  EventEndTurn(int oldPlayerID, int newPlayerID);
  ~EventEndTurn();

  int oldID() const;
  int newID() const;

  virtual void apply(Core& core) const;
  virtual bool isVisible(const Core& core) const;

  static EventEndTurn* generate(const Core &core);

private:
  int mOldID;
  int mNewID;
};

#endif
