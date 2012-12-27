// See LICENSE file for copyright and license details.

#ifndef CORE_END_TURN_H
#define CORE_END_TURN_H

typedef struct {
  int oldID;
  int newID;
} EventEndturn;

void generateEventEndTurn(Core& core);
void applyEventEndTurn(Core& core, const EventEndturn& e);

#endif
