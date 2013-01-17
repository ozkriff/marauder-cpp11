// See LICENSE file for copyright and license details.

#ifndef CORE__EVENT__EVENT_END_TURN_HPP
#define CORE__EVENT__EVENT_END_TURN_HPP

#include "core/event.hpp"

class EventEndTurn : public Event {
private:
  int mOldID;
  int mNewID;

public:
  EventEndTurn(int eventID, int oldPlayerID, int newPlayerID);
  ~EventEndTurn();

  int oldID() const;
  int newID() const;

  virtual void apply(Core& core);
  virtual void undo(Core& core);
  virtual bool isVisible(const Core& core) const;
};

void generateEventEndTurn(Core& core);

#endif
