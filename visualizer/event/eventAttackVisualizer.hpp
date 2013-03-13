// See LICENSE file for copyright and license details.

#ifndef VISUALIZER__EVENT__EVENT_ATTACK_VISUALIZER_HPP
#define VISUALIZER__EVENT__EVENT_ATTACK_VISUALIZER_HPP

#include "core/event/eventAttack.hpp"
#include "visualizer/eventVisualizer.hpp"

class EventAttackView;

class EventAttackVisualizer : public EventVisualizer {
public:
  EventAttackVisualizer(Visualizer& visualizer, const EventAttackView& event);
  virtual ~EventAttackVisualizer();

  virtual bool isFinished() const override;
  virtual void draw() override;
  virtual void end() override;

private:
  const EventAttackView& mEventAttack;
  int mFrame;
  int mLastFrame;
  float mFallingDownSpeed;
  V3f mInitialAttackerPosition;
  V3f mInitialVictimPosition;

  void drawLineOfFire();
};

#endif
