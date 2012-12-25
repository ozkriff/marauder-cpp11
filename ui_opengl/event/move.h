/* See LICENSE file for copyright and license details. */

#ifndef UI_OPENGL_EVENT_MOVE_H
#define UI_OPENGL_EVENT_MOVE_H

int get_last_event_move_index(const Event *e);
void get_current_moving_nodes(V2i *from, V2i *to);
void draw_moving_unit(void);

#endif
