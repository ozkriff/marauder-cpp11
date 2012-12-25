/* See LICENSE file for copyright and license details. */

#ifndef UI_OPENGL_EVENT_H
#define UI_OPENGL_EVENT_H

int get_last_event_index(const Event *e);
bool event_filter_unit(const Event *e, const Unit *u);
void event_draw(const Event *e);

#endif
