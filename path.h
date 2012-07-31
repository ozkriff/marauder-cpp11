/* See LICENSE file for copyright and license details. */

void init_pathfinding_module(void);
int get_path_length(V2i pos);
void get_path(V2i *path, int length, V2i pos);
void fill_map(const Unit *u);
void clean_map(void);
