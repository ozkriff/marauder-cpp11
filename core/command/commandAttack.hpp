// See LICENSE file for copyright and license details.

#ifndef CORE__COMMAND__COMMAND_ATTACK_HPP
#define CORE__COMMAND__COMMAND_ATTACK_HPP

#include "core/command.hpp"

class CommandAttack : public Command {
public:
  CommandAttack(int attackerID, int victimID);
  virtual ~CommandAttack() {}

  int mAttackerID;
  int mVictimID;
};

#endif
