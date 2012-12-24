/* See LICENSE file for copyright and license details. */

class Camera {
public:
  float x_angle;
  float z_angle;
  V2f pos;
  float zoom;

public:
  Camera();
  ~Camera();

  void set();
  void move(Dir d);
};
