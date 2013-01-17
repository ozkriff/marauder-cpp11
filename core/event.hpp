// See LICENSE file for copyright and license details.

#ifndef CORE__EVENT_HPP
#define CORE__EVENT_HPP

class Core;

enum class EventTypeID {
  MOVE,
  END_TURN
};

class Event {
private:
  EventTypeID mType;
  int mID;

public:
  Event(int id, EventTypeID type);
  virtual ~Event();

  EventTypeID type() const;
  int id() const;

  virtual void apply(Core& core) = 0;
  virtual void undo(Core& core) = 0;
  virtual bool isVisible(const Core& core) const = 0;
};

#endif
