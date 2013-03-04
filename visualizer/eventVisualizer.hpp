// See LICENSE file for copyright and license details.

#ifndef VISUALIZER__EVENT_VISUALIZER_HPP
#define VISUALIZER__EVENT_VISUALIZER_HPP

#include "visualizer/visualizer.hpp"

class EventVisualizer {
public:
  EventVisualizer(Visualizer& visualizer);
  virtual ~EventVisualizer();

  virtual bool isFinished() const = 0;
  virtual void draw() = 0;
  virtual void end() = 0;

protected:
  const Visualizer& visualizer() const;
  Visualizer& visualizer();

private:
  Visualizer& mVisualizer;
};

EventVisualizer* newEventVisualizer(
    Visualizer& visualizer, const EventView& eventView);

#endif
