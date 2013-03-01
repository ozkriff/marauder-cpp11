// See LICENSE file for copyright and license details.

#ifndef VISUALIZER__SCENE_MANAGER_HPP
#define VISUALIZER__SCENE_MANAGER_HPP

#include <map>
#include "visualizer/sceneNode.hpp"

class SceneManager {
public:
  SceneManager();
  ~SceneManager();

  void addNode(int nodeID, SceneNode* node);
  void deleteNode(int nodeID);
  SceneNode& sceneNode(int nodeID);

  std::map<int, SceneNode*>& nodes();

  void draw() const;

private:
  std::map<int, SceneNode*> mNodes;
};

#endif
