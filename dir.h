/* See LICENSE file for copyright and license details. */

typedef enum {
  D_N, D_NE, D_E, D_SE,
  D_S, D_SW, D_W, D_NW,
  D_NONE, D_ERROR, D_COUNT
} Dir;

extern V2i dir_to_pos_diff[8];

void neib(V2i *neib, const V2i* pos, Dir i);
Dir m2dir(const V2i *a, const V2i *b);
bool dir_is_diagonal(Dir d);
void get_dir_neib(V2i *n,
    const V2i *p1, const V2i *p2, int add_me);
