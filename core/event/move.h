/* See LICENSE file for copyright and license details. */

#ifndef CORE_MOVE_H
#define CORE_MOVE_H

typedef struct {
  int unit_id;
  V2i *path;
  int length;
  Dir initial_direction;
} EventMove;

void generate_event_move(
    const Unit *u, const V2i *destination);
void apply_event_move(const EventMove *e);
void undo_event_move(const EventMove *e);
bool is_visible_event_move(const EventMove *e);

#endif
