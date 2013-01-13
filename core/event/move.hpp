// See LICENSE file for copyright and license details.

#ifndef CORE_MOVE_H
#define CORE_MOVE_H

#include <vector>
#include "core/v2i.hpp"
#include "core/dir.hpp"
#include "core/event.hpp"

struct Unit;
class Core;

class EventMove : public Event {
public:
  int unitID;
  std::vector<V2i> path;
  int cost;
  Dir initialDirection;

public:
  EventMove(int id);
  virtual ~EventMove();

  virtual void apply(Core& core);
  virtual void undo(Core& core);
  virtual bool isVisible(const Core& core) const;
};

void generateEventMove(
    Core& core, const Unit& u, const V2i& destination);

#endif
