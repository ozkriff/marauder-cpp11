// See LICENSE file for copyright and license details.

#include "core/command/commandAttack.hpp"

CommandAttack::CommandAttack(UnitID attackerID, UnitID victimID)
  : Command(CommandType::Attack),
    mAttackerID(attackerID),
    mVictimID(victimID)
{}

CommandAttack::~CommandAttack() {
}

UnitID CommandAttack::attackerID() const {
  return mAttackerID;
}

UnitID CommandAttack::victimID() const{
  return mVictimID;
}
