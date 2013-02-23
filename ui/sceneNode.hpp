// See LICENSE file for copyright and license details.

#ifndef UI__SCENE_NODE_HPP
#define UI__SCENE_NODE_HPP

#include "ui/vertexArray.hpp"
#include "ui/color.hpp"

class SceneNode {
public:
  SceneNode();
  ~SceneNode();

  void draw() const;

  Color mColor;
  VertexArray* mVertexArray;
  V3f mPosition;
  float mRotationAngle;
private:
};

#endif
