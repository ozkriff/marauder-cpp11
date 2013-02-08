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

  Event& currentEvent();

  void switchToNextEvent();
  void addEvent(Event* e);

  bool unshownEventsLeft();
  void applyCurrentEvent();
  void undoUnshownEvents();
  int getNewEventID();

private:
  Core& mCore;
  std::list<Event*> mEvents;
  Event* mCurrentEvent;

  Event* getNextEvent();
  void applyEvent(Event& e);
  void applyInvisibleEvents();
  bool isEventVisible(const Event& e) const;
  void undoEvent(Event& e);
  Event* getNextEventNode();
  void event2log(const Event& e);
  void sendEvent(const Event& e);
};

#endif
