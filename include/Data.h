#pragma once

#include "BVHNode.h"
#include "Camera.h"
#include "Scene.h"

class Data {
public:
  Data() = default;

  void update(const Camera &camera);
  void update(const Scene &scene);
  void updateBVH(const Scene &scene, int maxTrianglesInNode);

private:
  void updateNode(BVHNode *node);
  void updateLeafNode(BVHNode *node);
  void add(const glm::vec3 &vec);
  void add(const float &value);
  void add(const int &value);
  void add(const bool bol);

private:
  float mData[10000000];
  int mOffset = 0;
};
