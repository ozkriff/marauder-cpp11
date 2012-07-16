/* See LICENSE file for copyright and license details. */

void init_pathfinding_module(void);
void get_path(List *path, V2i pos);

/* TODO remove from public functions. */
void fill_map(const Unit *u);

/* TODO remove from public functions. */
void clean_map(void);
