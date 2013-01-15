// See LICENSE file for copyright and license details.

#ifndef UI_OPENGL_CAMERA_H
#define UI_OPENGL_CAMERA_H

#include "ui/v2f.hpp"

class Camera {
private:
  float mMaxXAxisAngle;
  float mMinXAxisAngle;
  float mXAngle;
  float mZAngle;
  V2f mMaxPos;
  V2f mPos;
  float mMaxZoom;
  float mMinZoom;
  float mZoom;

  void clampPosition(const V2f& max);

public:
  Camera();
  ~Camera();

  void setMaxXAxisAngle(float angle);
  void setMinXAxisAngle(float angle);
  void setXAxisAngle(float angle);
  void rotateAroundXAxis(float angle);

  void setZAxisAngle(float angle);
  void rotateAroundZAxis(float angle);

  void setMaxPos(const V2f& maxPos);
  void setPos(const V2f& pos);
  void move(float angle);

  void setMaxZoom(float n);
  void setMinZoom(float n);
  void setZoom(float zoom);
  void zoomIn(float n);
  void zoomOut(float n);

  void set();
};

#endif
