/* See LICENSE file for copyright and license details. */

typedef struct {
  float x_angle;
  float z_angle;
  V2f pos;
  float zoom;
} Camera;

void set_camera(const Camera *c);
void move_camera(Camera *c, Dir d);
