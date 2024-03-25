#pragma once

#include "Mesh.h"
#include <glm/glm.hpp>

#define RESOLUTION 30

enum Mode { Position = 0, Rotation, Scale };

struct CoordinateSystem {
  glm::vec3 mPosition = glm::vec3(0);
  glm::vec3 mRotation = glm::vec3(0);

  // Rotation
  Vertex mRotX[RESOLUTION];
  Vertex mRotY[RESOLUTION];
  Vertex mRotZ[RESOLUTION];

  Vertex mGrabX;
  Vertex mGrabY;
  Vertex mGrabZ;

  // Position & Scale
  Vertex mCenter;
  Vertex mAxisX;
  Vertex mAxisY;
  Vertex mAxisZ;

  float mSize;
  Mode mMode;

  CoordinateSystem();
  void recalculate(Vertex &vertex, bool pos, bool rot);
  void update();
};
