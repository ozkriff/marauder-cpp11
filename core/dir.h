/* See LICENSE file for copyright and license details. */

#ifndef CORE_DIR_H
#define CORE_DIR_H

enum class Dir {
  D_N, D_NE, D_E, D_SE,
  D_S, D_SW, D_W, D_NW,
  D_NONE, D_ERROR, D_COUNT
};

V2i neib(const V2i& pos, Dir i);
Dir m2dir(const V2i& a, const V2i& b);
bool dir_is_diagonal(Dir d);
V2i get_dir_neib(const V2i& p1, const V2i& p2, int add_me);
Dir opposite_dir(Dir d);
int dir_diff(Dir d0, Dir d1);

#endif
