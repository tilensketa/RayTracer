#include "CoordinateSystem.h"
#include "glm/gtx/quaternion.hpp"

#include <glm/gtx/euler_angles.hpp>

CoordinateSystem::CoordinateSystem() {
  mSize = 0.3f;
  mMode = Mode::Position;

  mCenter.mPosition = glm::vec3(0);
  mPositionAxis[0].mPosition = glm::vec3(mSize, 0, 0);
  mPositionAxis[1].mPosition = glm::vec3(0, mSize, 0);
  mPositionAxis[2].mPosition = glm::vec3(0, 0, mSize);

  mScaleAxis[0].mPosition = glm::vec3(mSize, 0, 0);
  mScaleAxis[1].mPosition = glm::vec3(0, mSize, 0);
  mScaleAxis[2].mPosition = glm::vec3(0, 0, mSize);

  float mSize3 = mSize / 3.0f;
  float mSize2 = 2 * mSize / 3.0f;
  float mSizeMid = mSize / 2.0f;
  mPlaneXY[0].mPosition = glm::vec3(mSize3, mSize3, 0);
  mPlaneXY[1].mPosition = glm::vec3(mSize3, mSize2, 0);
  mPlaneXY[2].mPosition = glm::vec3(mSize2, mSize2, 0);
  mPlaneXY[3].mPosition = glm::vec3(mSize2, mSize3, 0);
  mPlaneXY[4].mPosition = glm::vec3(mSizeMid, mSizeMid, 0);

  mPlaneXZ[0].mPosition = glm::vec3(mSize3, 0, mSize3);
  mPlaneXZ[1].mPosition = glm::vec3(mSize3, 0, mSize2);
  mPlaneXZ[2].mPosition = glm::vec3(mSize2, 0, mSize2);
  mPlaneXZ[3].mPosition = glm::vec3(mSize2, 0, mSize3);
  mPlaneXZ[4].mPosition = glm::vec3(mSizeMid, 0, mSizeMid);

  mPlaneYZ[0].mPosition = glm::vec3(0, mSize3, mSize3);
  mPlaneYZ[1].mPosition = glm::vec3(0, mSize3, mSize2);
  mPlaneYZ[2].mPosition = glm::vec3(0, mSize2, mSize2);
  mPlaneYZ[3].mPosition = glm::vec3(0, mSize2, mSize3);
  mPlaneYZ[4].mPosition = glm::vec3(0, mSizeMid, mSizeMid);

  mRotationGrab[2].mPosition = glm::vec3(mSize, 0, 0);
  mRotationGrab[0].mPosition = glm::vec3(0, mSize, 0);
  mRotationGrab[1].mPosition = glm::vec3(0, 0, mSize);

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

  for (int i = 0; i < 3; i++) {
    recalculate(mScaleAxis[i], true, true);
    recalculate(mPositionAxis[i], true, false);
    recalculate(mRotationGrab[i], true, true);
  }

  for (int i = 0; i < 5; i++) {
    recalculate(mPlaneXY[i], true, false);
    recalculate(mPlaneXZ[i], true, false);
    recalculate(mPlaneYZ[i], true, false);
  }

  for (int i = 0; i < RESOLUTION; i++) {
    recalculate(mRotX[i], true, true);
    recalculate(mRotY[i], true, true);
    recalculate(mRotZ[i], true, true);
  }
}
