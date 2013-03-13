// See LICENSE file for copyright and license details.

#ifndef CORE__EVENT__EVENT_END_TURN_HPP
#define CORE__EVENT__EVENT_END_TURN_HPP

#include "core/event.hpp"
#include "core/player.hpp"

class EventEndTurn : public Event {
public:
  EventEndTurn(PlayerID oldPlayerID, PlayerID newPlayerID);
  ~EventEndTurn();

  PlayerID oldID() const;
  PlayerID newID() const;

  virtual void apply(Core& core) const override;
  virtual bool isVisible(const Core& core) const override;

  static EventEndTurn* generate(const Core &core);

private:
  PlayerID mOldID;
  PlayerID mNewID;
};

#endif
