#pragma once

#include "Camera.h"
#include "Scene.h"

struct Data {
  float mData[100000];

  void update(const Camera &camera);
  void update(const Scene &scene);
};
