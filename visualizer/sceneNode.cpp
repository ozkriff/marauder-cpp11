// See LICENSE file for copyright and license details.

#include "visualizer/sceneNode.hpp"
#include <cassert>
#include "SDL_opengl.h"
#include "visualizer/mesh.hpp"

SceneNode::SceneNode()
  : mColor(1, 1, 1),
    mMesh(nullptr),
    mPosition(0.0f, 0.0f, 0.0f),
    mRotationAngle(0.0f)
{
}

SceneNode::~SceneNode() {
  for (auto* child : mChildrens) {
    assert(child != nullptr);
    delete child;
  }
}

void SceneNode::draw() const {
  glColor3f(mColor.red(), mColor.green(), mColor.blue());
  glPushMatrix();
  glTranslatef(mPosition.x(), mPosition.y(), mPosition.z());
  glRotatef(mRotationAngle, 0, 0, 1);
  for (const auto* child : mChildrens) {
    assert(child != nullptr);
    child->draw();
  }
  assert(mMesh);
  mMesh->draw();
  glPopMatrix();
}
