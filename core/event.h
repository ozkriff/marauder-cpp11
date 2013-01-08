// See LICENSE file for copyright and license details.

#ifndef CORE_EVENT_H
#define CORE_EVENT_H

class Core;

enum class EventTypeID {
  MOVE,
  END_TURN
};

class Event {
public:
  EventTypeID t;
  int id;

  virtual void apply(Core& core) = 0;
  virtual void undo(Core& core) = 0;
  virtual bool isVisible(const Core& core) const = 0;
};

#endif
