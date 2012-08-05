/* See LICENSE file for copyright and license details. */

typedef struct {
  int old_id;
  int new_id;
} EventEndturn;

void generate_event_end_turn(void);
void apply_event_end_turn(const EventEndturn *e);
