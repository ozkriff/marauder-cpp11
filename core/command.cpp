// See LICENSE file for copyright and license details.

#include "core/command.hpp"

Command::Command(CommandType type)
  : mType(type)
{}

Command::~Command() {
}

CommandType Command::type() const {
  return mType;
}
