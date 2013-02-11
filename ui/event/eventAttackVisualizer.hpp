// See LICENSE file for copyright and license details.

#ifndef UI__EVENT__EVENT_ATTACK_VISUALIZER_HPP
#define UI__EVENT__EVENT_ATTACK_VISUALIZER_HPP

#include "core/event/eventAttack.hpp"
#include "ui/eventVisualizer.hpp"

class Event;

class EventAttackVisualizer : public EventVisualizer {
public:
  EventAttackVisualizer(Game& game, const Event& event);
  virtual ~EventAttackVisualizer();

  virtual bool isFinished();
  virtual bool isUnitVisible(const Unit& u);
  virtual void draw();
  virtual void end();

private:
  const EventAttack& mEventAttack;
  int mFrame;

  void drawLineOfFire();
};

#endif
