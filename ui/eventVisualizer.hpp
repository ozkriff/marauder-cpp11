// See LICENSE file for copyright and license details.

#ifndef UI__EVENT_VISUALIZER_HPP
#define UI__EVENT_VISUALIZER_HPP

#include "ui/game.hpp"

class EventVisualizer {
private:
  Game& mGame;

protected:
  const Game& game() const;
  Game& game();

public:
  EventVisualizer(Game& game);
  virtual ~EventVisualizer();

  virtual bool isFinished() = 0;
  virtual bool isUnitVisible(const Unit& u) = 0;
  virtual void draw() = 0;
  virtual void end() = 0;
};

EventVisualizer* newEventVisualizer(Game& game, const Event& event);

#endif
