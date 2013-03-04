// See LICENSE file for copyright and license details.

#ifndef CORE__COMMAND_HPP
#define CORE__COMMAND_HPP

#include <vector>
#include "core/v2i.hpp"

enum class CommandType {
  Move,
  EndTurn,
  Attack
};

class Command {
public:
  Command(CommandType type);
  virtual ~Command();

  CommandType type() const;

private:
  CommandType mType;
};

#endif
