// See LICENSE file for copyright and license details.

#ifndef VISUALIZER__CAMERA_HPP
#define VISUALIZER__CAMERA_HPP

#include "visualizer/v2f.hpp"

class Camera {
public:
  Camera();
  ~Camera();

  void setMaxXAxisAngle(float angle);
  void setMinXAxisAngle(float angle);
  void setXAxisAngle(float angle);
  void rotateAroundXAxis(float angle);

  void setZAxisAngle(float angle);
  void rotateAroundZAxis(float angle);

  void setMaxPosition(const V2f& maxPosition);
  void setPosition(const V2f& position);
  void move(float angle);

  void setMaxZoom(float n);
  void setMinZoom(float n);
  void setZoom(float zoom);
  void zoomIn(float n);
  void zoomOut(float n);

  void set();

private:
  float mMaxXAxisAngle;
  float mMinXAxisAngle;
  float mXAngle;
  float mZAngle;
  V2f mMaxPosition;
  V2f mPosition;
  float mMaxZoom;
  float mMinZoom;
  float mZoom;

  float speed() const;
  void clampPosition();
};

#endif
