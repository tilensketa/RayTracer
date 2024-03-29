#pragma once

#include "Mesh.h"
#include <glm/glm.hpp>

#define RESOLUTION 30

enum Mode { Position = 0, Rotation, Scale };

struct CoordinateSystem {
  glm::vec3 mPosition = glm::vec3(0);
  glm::vec3 mRotation = glm::vec3(0);

  Vertex mCenter;

  // Rotation
  Vertex mRotationGrab[3];
  Vertex mRotX[RESOLUTION];
  Vertex mRotY[RESOLUTION];
  Vertex mRotZ[RESOLUTION];

  // Position
  Vertex mPositionAxis[3];
  Vertex mPlaneXY[5];
  Vertex mPlaneXZ[5];
  Vertex mPlaneYZ[5];

  // Scale
  Vertex mScaleAxis[3];

  float mSize;
  Mode mMode;

  CoordinateSystem();
  void recalculate(Vertex &vertex, bool pos, bool rot);
  void update();
};
