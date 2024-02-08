#pragma once

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

class Camera {
public:
  Camera() = default;
  Camera(int width, int height, float fov);

  bool update(GLFWwindow *window, float ts);
  void setResolution(int width, int height);

  const float getConfig() const { return mBlack; }
  const glm::ivec2 &getResolution() const { return mResolution; }
  const float getFOV() const { return mFOV; }
  const glm::vec3 &getPosition() const { return mPosition; }
  const glm::vec3 &getFront() const { return mFront; }

private:
  float mBlack = 1.0f;
  glm::ivec2 mResolution;
  float mFOV;
  glm::vec3 mPosition;
  glm::vec3 mFront;
  glm::vec3 mUp;

  float mMoveSpeed = 5;
  float mRotateSpeed = 1;
  glm::vec2 mLastMousePosition;
};
