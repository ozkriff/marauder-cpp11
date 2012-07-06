/* See LICENSE file for copyright and license details. */

void init_pathfinding_module(void);
List get_path(V2i pos);

/* TODO remove from public functions. */
void fill_map(const V2i *pos);

/* TODO remove from public functions. */
void clean_map(void);
