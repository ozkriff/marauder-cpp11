// See LICENSE file for copyright and license details.

#ifndef CORE_END_TURN_H
#define CORE_END_TURN_H

typedef struct {
  int old_id;
  int new_id;
} EventEndturn;

void generate_event_end_turn(Core& core);
void apply_event_end_turn(Core& core, const EventEndturn& e);

#endif
