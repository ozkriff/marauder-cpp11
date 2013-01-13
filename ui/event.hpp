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

  // TODO: Rename: framesCount?
  virtual int lastIndex() = 0;
  virtual bool filterUnit(const Unit& u) = 0;
  virtual void draw() = 0;
};

EventVisualizer* newEventVisualizer(Game& game, const Event& event);

#endif
