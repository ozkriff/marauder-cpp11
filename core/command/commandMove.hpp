// See LICENSE file for copyright and license details.

#ifndef CORE__COMMAND__COMMAND_MOVE_HPP
#define CORE__COMMAND__COMMAND_MOVE_HPP

#include "core/command.hpp"

class CommandMove : public Command {
public:
  // CommandMove(int unitID, const std::vector<V2i>& path);
  CommandMove(int unitID, V2i destination);
  virtual ~CommandMove() {}

  int mUnitID;
  V2i mDestination;
  // std::vector<V2i> mPath;
};

#endif
