#include "Camera.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>

Camera::Camera(int width, int height, float fov) {
  mBlack = 1.0f;
  mResolution = glm::ivec2(width, height);
  mFOV = fov;
  mPosition = glm::vec3(0, 0, 5);
  mFront = glm::vec3(0, 0, -1);
  mUp = glm::vec3(0, 1, 0);
}

bool Camera::update(GLFWwindow *window, float ts) {

  bool moved = false;
  bool rotated = false;
  bool updated = false;

  double x;
  double y;
  glfwGetCursorPos(window, &x, &y);
  glm::vec2 mousePos(x, y);
  glm::vec2 delta = (mousePos - mLastMousePosition) * 0.002f;
  mLastMousePosition = mousePos;

  if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    return false;
  }

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glm::vec3 right = glm::cross(mFront, mUp);

  // Position
  if (glfwGetKey(window, GLFW_KEY_W)) {
    mPosition += mFront * mMoveSpeed * ts;
    moved = true;
  } else if (glfwGetKey(window, GLFW_KEY_S)) {
    mPosition -= mFront * mMoveSpeed * ts;
    moved = true;
  }

  if (glfwGetKey(window, GLFW_KEY_A)) {
    mPosition -= right * mMoveSpeed * ts;
    moved = true;
  } else if (glfwGetKey(window, GLFW_KEY_D)) {
    mPosition += right * mMoveSpeed * ts;
    moved = true;
  }

  if (glfwGetKey(window, GLFW_KEY_SPACE)) {
    mPosition += mUp * mMoveSpeed * ts;
    moved = true;
  } else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) {
    mPosition -= mUp * mMoveSpeed * ts;
    moved = true;
  }

  // Rotation
  if (delta.x != 0.0f || delta.y != 0.0f) {
    float pitchDelta = delta.y * mRotateSpeed;
    float yawDelta = delta.x * mRotateSpeed;

    glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, right),
                                            glm::angleAxis(-yawDelta, mUp)));
    mFront = glm::rotate(q, mFront);
    rotated = true;
  }

  if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
    mBlack = 1.0f;
    updated = true;
  }
  if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
    mBlack = 0.0f;
    updated = true;
  }

  if (moved || rotated || updated)
    return true;

  return false;
}

void Camera::setResolution(int width, int height) {
  mResolution = glm::ivec2(width, height);
}
