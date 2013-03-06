// See LICENSE file for copyright and license details.

#ifndef CORE__EVENT_HPP
#define CORE__EVENT_HPP

class Core;

enum class EventType {
  Move,
  EndTurn,
  Attack
};

class Event {
public:
  Event(EventType type);
  virtual ~Event();

  EventType type() const;

  virtual void apply(Core& core) const = 0;
  virtual bool isVisible(const Core& core) const = 0;

private:
  EventType mType;
};

#endif
