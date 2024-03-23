#pragma once

#include "BVHNode.h"
#include "Camera.h"
#include "Scene.h"
#include "Settings.h"

class Data {
public:
  void updateCamera(const Camera &camera);

  void updateBVH(const Scene& scene, const Settings& settings);
  void updateLights(const Scene& scene);
  void updateSettings(const Settings& settings);
  void updateMaterial(const Scene& scene, bool alone);

  const int getFloatDataSize() const { return mDataFloatSize; }

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
  void add(const Light &light);

private:
  float mData[10000000];
  int mOffset = 0;
  int mDataFloatSize = 0;
};
