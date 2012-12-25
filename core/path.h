/* See LICENSE file for copyright and license details. */

#ifndef CORE_PATH_H
#define CORE_PATH_H

void init_pathfinding_module();
int get_path_length(V2i pos);
void get_path(V2i *path, int length, V2i pos);
void fill_map(const Unit *u);
void clean_map();

#endif
