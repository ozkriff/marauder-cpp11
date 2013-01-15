// See LICENSE file for copyright and license details.

#include <cassert>
#include <cmath>
#include <SDL/SDL_opengl.h>
#include "core/dir.hpp"
#include "ui/math.hpp"
#include "ui/camera.hpp"

Camera::Camera()
  : mMaxXAxisAngle(50.0f),
    mMinXAxisAngle(0.0f),
    mXAngle(45.0f),
    mZAngle(45.0f),
    mMaxPos(0.0f, 0.0f),
    mPos(0.0f, 0.0f),
    mMaxZoom(200.0f),
    mMinZoom(30.0f),
    mZoom(100.0f)
{
}

Camera::~Camera() {
}

void Camera::set() {
  glTranslatef(0, 0, -mZoom);
  glRotatef(mXAngle, -1, 0, 0);
  glRotatef(mZAngle, 0, 0, 1);
  glTranslatef(-mPos.x(), -mPos.y(), 0);
}

void Camera::move(float angle) {
  float speed = mZoom / 20.0f; // TODO: do not calc here
  float speedInRadians = deg2rad(mZAngle - angle);
  mPos.setX(mPos.x() + sin(speedInRadians) * speed);
  mPos.setY(mPos.y() + cos(speedInRadians) * speed);
  clampPosition();
}

void Camera::clampPosition() {
  const V2f& max = mMaxPos;
  if (mPos.x() > max.x()) {
    mPos.setX(max.x());
  } else if (mPos.x() < 0) {
    mPos.setX(0);
  }
  if (mPos.y() > max.y()) {
    mPos.setY(max.y());
  } else if (mPos.y() < 0) {
    mPos.setY(0);
  }
}

void Camera::setMaxXAxisAngle(float angle) {
  mMaxXAxisAngle = angle;
}

void Camera::setMinXAxisAngle(float angle) {
  mMinXAxisAngle = angle;
}

void Camera::setXAxisAngle(float angle) {
  mXAngle = clampF(angle, mMinXAxisAngle, mMaxXAxisAngle);
}

void Camera::rotateAroundXAxis(float angle) {
  setXAxisAngle(mXAngle + angle);
}

void Camera::setZAxisAngle(float angle) {
  mZAngle = clampAngle(angle);
}

void Camera::rotateAroundZAxis(float angle) {
  setZAxisAngle(mZAngle + angle);
}

void Camera::setMaxPos(const V2f& maxPos) {
  mMaxPos = maxPos;
}

void Camera::setPos(const V2f& pos) {
  mPos = pos;
  clampPosition();
}

void Camera::setMaxZoom(float n) {
  mMaxZoom = n;
}

void Camera::setMinZoom(float n) {
  mMinZoom = n;
}

void Camera::setZoom(float zoom) {
  mZoom = clampF(zoom, mMinZoom, mMaxZoom);
}

void Camera::zoomIn(float n) {
  setZoom(mZoom - n);
}

void Camera::zoomOut(float n) {
  setZoom(mZoom + n);
}
