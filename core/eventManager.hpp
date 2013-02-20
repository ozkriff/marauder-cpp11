// See LICENSE file for copyright and license details.

#ifndef CORE__EVENT_MANAGER_HPP
#define CORE__EVENT_MANAGER_HPP

#include <list>

class Core;
class Event;

class EventManager {
public:
  EventManager(Core& core);
  ~EventManager();

  const Event& currentEvent() const;

  void switchToNextEvent();
  void addEvent(Event* e);

  bool unshownEventsLeft();
  void applyCurrentEvent();
  void undoUnshownEvents();
  int getNewEventID() const;

private:
  Core& mCore;
  std::list<Event*> mEvents;
  Event* mCurrentEvent;

  Event* getNextEvent();
  void applyEvent(const Event& e);
  void applyInvisibleEvents();
  bool isEventVisible(const Event& e) const;
  const Event* getLastSeenEventNode();
  void event2log(const Event& e);
  void sendEvent(const Event& e);
};

#endif
