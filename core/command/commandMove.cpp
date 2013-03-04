// See LICENSE file for copyright and license details.

#include "core/command/commandMove.hpp"

CommandMove::CommandMove(int unitID, V2i destination)
  : Command(CommandType::Move),
    mUnitID(unitID),
    mDestination(destination)
{}
