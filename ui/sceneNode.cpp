#include "ui/sceneNode.hpp"
#include <cassert>

SceneNode::SceneNode()
  : mColor(1, 1, 1),
    mVertexArray(nullptr),
    mPosition(0.0f, 0.0f, 0.0f),
    mRotationAngle(0.0f)
{
}

SceneNode::~SceneNode() {
}

void SceneNode::draw() const {
  glColor3f(mColor.red(), mColor.green(), mColor.blue());
  glPushMatrix();
  glTranslatef(mPosition.x(), mPosition.y(), mPosition.z());
  glRotatef(mRotationAngle, 0, 0, 1);
  mVertexArray->draw();
  glPopMatrix();
}
