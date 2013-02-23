#include "ui/sceneManager.hpp"
#include <cassert>

SceneManager::SceneManager()
  : mNodes()
{
}

SceneManager::~SceneManager() {
}

void SceneManager::addNode(int nodeID, SceneNode* node) {
  mNodes[nodeID] = node;
}

SceneNode& SceneManager::sceneNode(int nodeID) {
  assert(mNodes.count(nodeID) == 1);
  return *mNodes.at(nodeID);
}

void SceneManager::deleteNode(int nodeID) {
  mNodes.erase(nodeID);
}

void SceneManager::draw() const {
  for (auto pair: mNodes) {
    SceneNode* sceneNode = pair.second;
    sceneNode->draw();
  }
}
