// See LICENSE file for copyright and license details.

#ifndef CORE__EVENT__EVENT_ATTACK_HPP
#define CORE__EVENT__EVENT_ATTACK_HPP

#include <vector>
#include "core/v2i.hpp"
#include "core/dir.hpp"
#include "core/event.hpp"

class Unit;
class Core;

class EventAttack : public Event {
public:
  int mAttackerID;
  int mVictimID;

public:
  EventAttack(int id);
  virtual ~EventAttack();

  virtual void apply(Core& core) const;
  virtual void undo(Core& core) const;
  virtual bool isVisible(const Core& core) const;

  static void generate(
      Core& core, const Unit& attacker, const Unit& victim);
};

#endif
