// See LICENSE file for copyright and license details.

#ifndef CORE_END_TURN_H
#define CORE_END_TURN_H

#include "core/event.hpp"

class EventEndTurn : public Event {
public:
  int oldID;
  int newID;

  virtual void apply(Core& core);
  virtual void undo(Core& core);
  virtual bool isVisible(const Core& core) const;
};

void generateEventEndTurn(Core& core);

#endif
