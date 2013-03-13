// See LICENSE file for copyright and license details.

#ifndef CORE__EVENT__EVENT_ATTACK_HPP
#define CORE__EVENT__EVENT_ATTACK_HPP

#include <vector>
#include "core/event.hpp"
#include "core/unit.hpp"

class Unit;
class Core;

class EventAttack : public Event {
public:
  EventAttack();
  virtual ~EventAttack();

  UnitID victimID() const;
  UnitID attackerID() const;

  virtual void apply(Core& core) const override;
  virtual bool isVisible(const Core& core) const override;

  static EventAttack* generate(
      const Core& core, const Unit& attacker, const Unit& victim);

private:
  UnitID mAttackerID;
  UnitID mVictimID;
};

#endif
