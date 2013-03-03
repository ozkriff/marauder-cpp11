// See LICENSE file for copyright and license details.

#ifndef CORE__COMMAND_HPP
#define CORE__COMMAND_HPP

#include <vector>

#include "core/v2i.hpp" // TODO: ?!

class V2i;

enum class CommandType {
  Move,
  EndTurn,
  Attack
};

class Command {
public:
  // TODO: ...
  Command(CommandType type);
  virtual ~Command() {}

  CommandType mType;
};

class CommandMove : public Command {
public:
  // CommandMove(int unitID, const std::vector<V2i>& path);
  CommandMove(int unitID, V2i destination);
  virtual ~CommandMove() {}

  int mUnitID;
  V2i mDestination;
  // std::vector<V2i> mPath;
};

class CommandEndTurn : public Command {
public:
  CommandEndTurn();
  virtual ~CommandEndTurn() {}
};

class CommandAttack : public Command {
public:
  CommandAttack(int attackerID, int victimID);
  virtual ~CommandAttack() {}

  int mAttackerID;
  int mVictimID;
};

#endif
