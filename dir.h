/* See LICENSE file for copyright and license details. */

/* up right down left */
typedef enum {
  D_UP, D_UP_RIGHT, D_RIGHT, D_DOWN_RIGHT,
  D_DOWN, D_DOWN_LEFT, D_LEFT, D_UP_LEFT,
  D_NONE, D_ERROR, D_COUNT
} Dir;

extern V2i dir_to_pos_diff[8];

void neib(V2i *neib, const V2i* pos, Dir i);
Dir m2dir(const V2i *a, const V2i *b);
