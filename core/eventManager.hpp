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

  void addEvent(Event* e);

  bool isEmpty();
  Event* popEvent();

private:
  std::list<Event*> mNewEvents;
};

#endif
