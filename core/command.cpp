// See LICENSE file for copyright and license details.

#include "core/command.hpp"

Command::Command(CommandType type)
  : mType(type)
{}

CommandType Command::type() const {
  return mType;
}

CommandMove::CommandMove(int unitID, V2i destination)
  : Command(CommandType::Move),
    mUnitID(unitID),
    mDestination(destination)
{}

CommandEndTurn::CommandEndTurn()
  : Command(CommandType::EndTurn)
{}

CommandAttack::CommandAttack(int attackerID, int victimID)
  : Command(CommandType::Attack),
    mAttackerID(attackerID),
    mVictimID(victimID)
{}
