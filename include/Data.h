#pragma once

#include "BVHNode.h"
#include "Camera.h"
#include "Scene.h"

class Data {
public:
  Data() = default;

  void updateCamera(const Camera &camera);
  void updateScene(Scene &scene, int maxDepth);

private:
  void updateNode(BVHNode *node);
  void updateLeafNode(BVHNode *node);

  void add(const bool bol);
  void add(const float &value);
  void add(const int &value);
  void add(const glm::vec2 &vec);
  void add(const glm::ivec2 &vec);
  void add(const glm::vec3 &vec);
  void add(const glm::mat3 &mat);
  void add(const Vertex &vertex);
  void add(const Triangle &triangle);
  void add(const Material &material);

private:
  float mData[10000000];
  int mOffset = 0;
};
