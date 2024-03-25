#include "CoordinateSystem.h"

#include <glm/gtx/euler_angles.hpp>

CoordinateSystem::CoordinateSystem() {
  mSize = 0.3f;
  mMode = Mode::Position;

  mCenter.mPosition = glm::vec3(0);
  mAxisX.mPosition = glm::vec3(mSize, 0, 0);
  mAxisY.mPosition = glm::vec3(0, mSize, 0);
  mAxisZ.mPosition = glm::vec3(0, 0, mSize);

  mGrabZ.mPosition = glm::vec3(mSize, 0, 0);
  mGrabX.mPosition = glm::vec3(0, mSize, 0);
  mGrabY.mPosition = glm::vec3(0, 0, mSize);

  float deltaAngle = 360.0f / RESOLUTION;
  for (int i = 0; i < RESOLUTION; i++) {
    float angle = glm::radians(i * deltaAngle);
    float cos = glm::cos(angle);
    float sin = glm::sin(angle);

    mRotX[i].mPosition = mSize * glm::vec3(0, cos, sin);
    mRotY[i].mPosition = mSize * glm::vec3(cos, 0, sin);
    mRotZ[i].mPosition = mSize * glm::vec3(cos, sin, 0);
  }
  update();
}

void CoordinateSystem::recalculate(Vertex &vertex, bool pos, bool rot) {

  // Scale
  vertex.mModedPosition = vertex.mPosition;
  // Position
  if (pos)
    vertex.mModedPosition += mPosition;
  // Rotate
  if (rot) {
    glm::mat3 rotationMatrix =
        glm::eulerAngleXYZ(glm::radians(mRotation.x), glm::radians(mRotation.y),
                           glm::radians(mRotation.z));
    glm::vec3 o = vertex.mModedPosition - mPosition;
    vertex.mModedPosition = mPosition + rotationMatrix * o;
  }
}

void CoordinateSystem::update() {
  recalculate(mCenter, true, false);
  recalculate(mAxisX, true, false);
  recalculate(mAxisY, true, false);
  recalculate(mAxisZ, true, false);

  recalculate(mGrabX, true, true);
  recalculate(mGrabY, true, true);
  recalculate(mGrabZ, true, true);
  for (int i = 0; i < RESOLUTION; i++) {
    recalculate(mRotX[i], true, true);
    recalculate(mRotY[i], true, true);
    recalculate(mRotZ[i], true, true);
  }
}
