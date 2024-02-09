#pragma once

#include "Camera.h"
#include "Scene.h"

class Data {
public:
  Data() = default;

  void update(const Camera &camera);
  void update(const Scene &scene);

private:
  void add(const glm::vec3 &vec);
  void add(const float &value);
  void add(const int &value);

private:
  float mData[10000000];
  int mOffset = 0;
};
