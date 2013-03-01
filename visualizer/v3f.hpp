// See LICENSE file for copyright and license details.

#ifndef VISUALIZER__V3F_HPP
#define VISUALIZER__V3F_HPP

#include "visualizer/v2f.hpp"

class V3f {
public:
  V3f(float x = 0.0f, float y = 0.0f, float z = 0.0f);
  V3f(const V2f& v2f, float z = 0.0f);

  float x() const;
  float y() const;
  float z() const;

  void setX(float x);
  void setY(float y);
  void setZ(float z);

private:
  float mX;
  float mY;
  float mZ;
};

#endif
