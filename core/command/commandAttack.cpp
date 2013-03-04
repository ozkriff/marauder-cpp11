// See LICENSE file for copyright and license details.

#include "core/command/commandAttack.hpp"

CommandAttack::CommandAttack(int attackerID, int victimID)
  : Command(CommandType::Attack),
    mAttackerID(attackerID),
    mVictimID(victimID)
{}

int CommandAttack::attackerID() const {
  return mAttackerID;
}

int CommandAttack::victimID() const{
  return mVictimID;
}
