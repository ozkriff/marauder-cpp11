// See LICENSE file for copyright and license details.

#ifndef VISUALIZER__SCENE_NODE_HPP
#define VISUALIZER__SCENE_NODE_HPP

#include <list>
#include "visualizer/color4f.hpp"
#include "visualizer/v3f.hpp"

class Mesh;

class SceneNode {
public:
  SceneNode();
  ~SceneNode();

  void draw() const;

  Color4f mColor;
  Mesh* mMesh;
  V3f mPosition;
  float mRotationAngle;
  std::list<SceneNode*> mChildrens;

private:
};

#endif
