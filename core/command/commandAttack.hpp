// See LICENSE file for copyright and license details.

#ifndef CORE__COMMAND__COMMAND_ATTACK_HPP
#define CORE__COMMAND__COMMAND_ATTACK_HPP

#include "core/command.hpp"
#include "core/unit.hpp"

class CommandAttack : public Command {
public:
  CommandAttack(UnitID attackerID, UnitID victimID);
  virtual ~CommandAttack();

  UnitID attackerID() const;
  UnitID victimID() const;

private:
  UnitID mAttackerID;
  UnitID mVictimID;
};

#endif
