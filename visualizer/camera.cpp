// See LICENSE file for copyright and license details.

#include <cassert>
#include <cmath>
#include "SDL_opengl.h"
#include "core/dir.hpp"
#include "visualizer/math.hpp"
#include "visualizer/camera.hpp"

Camera::Camera()
  : mMaxXAxisAngle(50.0f),
    mMinXAxisAngle(0.0f),
    mXAngle(45.0f),
    mZAngle(45.0f),
    mMaxPosition(0.0f, 0.0f),
    mPosition(0.0f, 0.0f),
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
  glTranslatef(-mPosition.x(), -mPosition.y(), 0);
}

void Camera::move(float angle) {
  float speedInRadians = deg2rad(mZAngle - angle);
  mPosition.setX(mPosition.x() + sin(speedInRadians) * speed());
  mPosition.setY(mPosition.y() + cos(speedInRadians) * speed());
  clampPosition();
}

float Camera::speed() const {
  return mZoom / 40.0f;
}

void Camera::clampPosition() {
  mPosition.setX(clampF(mPosition.x(), 0.0f, mMaxPosition.x()));
  mPosition.setY(clampF(mPosition.y(), 0.0f, mMaxPosition.y()));
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

void Camera::setMaxPosition(const V2f& maxPosition) {
  mMaxPosition = maxPosition;
}

void Camera::setPosition(const V2f& position) {
  mPosition = position;
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
