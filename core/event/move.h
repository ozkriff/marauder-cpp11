// See LICENSE file for copyright and license details.

#ifndef CORE_MOVE_H
#define CORE_MOVE_H

#include <vector>
#include "core/v2i.h"

struct Unit;
class Core;

class EventMove {
public:
  int unitID;
  std::vector<V2i> path;
  int length;
  Dir initialDirection;

public:
  EventMove();
  ~EventMove();
};

void generateEventMove(
    Core& core, const Unit& u, const V2i& destination);
void applyEventMove(Core& core, const EventMove& e);
void undoEventMove(Core& core, const EventMove& e);
bool isVisibleEventMove(const Core& core, const EventMove& e);

#endif
