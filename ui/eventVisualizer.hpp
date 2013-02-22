// See LICENSE file for copyright and license details.

#ifndef UI__EVENT_VISUALIZER_HPP
#define UI__EVENT_VISUALIZER_HPP

#include "ui/game.hpp"

class EventVisualizer {
public:
  EventVisualizer(Game& game);
  virtual ~EventVisualizer();

  virtual bool isFinished() const = 0;
  virtual bool isUnitVisible(const Unit& u) const = 0;
  virtual void draw() = 0;
  virtual void end() = 0;

protected:
  const Game& game() const;
  Game& game();

private:
  Game& mGame;
};

EventVisualizer* newEventVisualizer(Game& game, const Event& event);

#endif
