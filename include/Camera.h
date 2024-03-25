#pragma once

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

class Camera {
public:
  Camera(int width, int height, float fov);

  bool update(GLFWwindow *window, float ts);

  // Resolution
  void setResolution(int width, int height);
  const glm::ivec2 &getResolution() const { return mResolution; }

  // FOV
  const float getFOV() const { return mFOV; }
  float &modFOV() { return mFOV; }

  // Position
  const glm::vec3 &getPosition() const { return mPosition; }

  // Direction
  const glm::vec3 &getFront() const { return mFront; }
  const glm::vec3 &getUp() const { return mUp; }

  // Aspect ratio
  const float getAspectRatio() const { return mAspectRatio; }

  // Matrix
  const glm::mat3 &getMatrix() const { return mMatrix; }

  // Speed
  float &modMoveSpeed() { return mMoveSpeed; }
  float &modRotateSpeed() { return mRotateSpeed; }

private:
  void recalculateMatrix();

private:
  glm::ivec2 mResolution;
  float mFOV;
  glm::vec3 mPosition;
  glm::vec3 mFront;
  glm::vec3 mUp;
  float mAspectRatio;
  glm::mat3 mMatrix;

  float mMoveSpeed = 5;
  float mRotateSpeed = 1;
  glm::vec2 mLastMousePosition;
};
