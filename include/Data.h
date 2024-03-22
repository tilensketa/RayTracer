#pragma once

#include "BVHNode.h"
#include "Camera.h"
#include "Scene.h"
#include "Settings.h"

class Data {
public:
  void update(const Camera &camera, Scene &scene, Settings &settings);
  void updateCamera(const Camera &camera, Settings &settings);
  void updateScene(Scene &scene, Settings &settings);

  const int getFloatDataSize() const { return mOffset; }

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
};
