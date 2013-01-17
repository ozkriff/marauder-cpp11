// See LICENSE file for copyright and license details.

#ifndef UI_OPENGL_EVENT_H
#define UI_OPENGL_EVENT_H

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

  virtual int framesCount() = 0;
  virtual bool isUnitVisible(const Unit& u) = 0;
  virtual void draw() = 0;
  virtual void end() = 0;
};

EventVisualizer* newEventVisualizer(Game& game, const Event& event);

#endif
