#include "Camera.h"
// #include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>

Camera::Camera(unsigned int width, unsigned int height,
               float fov) {
  Width = width;
  Height = height;
  FOV = fov;
  Position[0] = 0;
  Position[1] = 0;
  Position[2] = 5;
  Front[0] = 0;
  Front[1] = 0;
  Front[2] = -1;
}

bool Camera::update(GLFWwindow *window, float ts) {
  glm::vec3 position(Position[0], Position[1], Position[2]);
  glm::vec3 forward(Front[0], Front[1], Front[2]);
  glm::vec2 lastMousePosition(LastMousePosition[0], LastMousePosition[1]);
  glm::vec3 up(0, 1, 0);
  glm::vec3 right = glm::cross(forward, up);

  bool moved = false;
  bool rotated = false;

  double x;
  double y;
  glfwGetCursorPos(window, &x, &y);
  glm::vec2 mousePos(x, y);
  glm::vec2 delta = (mousePos - lastMousePosition) * 0.002f;
  lastMousePosition = mousePos;
  // std::cout << delta.x << " " << delta.y << std::endl;

  if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    return false;
  }

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Position
  if (glfwGetKey(window, GLFW_KEY_W)) {
    position += forward * MoveSpeed * ts;
    moved = true;
  } else if (glfwGetKey(window, GLFW_KEY_S)) {
    position -= forward * MoveSpeed * ts;
    moved = true;
  }

  if (glfwGetKey(window, GLFW_KEY_A)) {
    position -= right * MoveSpeed * ts;
    moved = true;
  } else if (glfwGetKey(window, GLFW_KEY_D)) {
    position += right * MoveSpeed * ts;
    moved = true;
  }

  if (glfwGetKey(window, GLFW_KEY_SPACE)) {
    position += up * MoveSpeed * ts;
    moved = true;
  } else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) {
    position -= up * MoveSpeed * ts;
    moved = true;
  }

  // Rotation
  if (delta.x != 0.0f || delta.y != 0.0f) {
    float pitchDelta = delta.y * RotateSpeed;
    float yawDelta = delta.x * RotateSpeed;

    glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, right),
                                            glm::angleAxis(-yawDelta, up)));
    forward = glm::rotate(q, forward);
    rotated = true;
  }

  if (moved)
    recalculatePosition(position);
  if (rotated)
    recalculateRotation(forward, lastMousePosition);

  if (moved || rotated)
    return true;

  return false;
}

void Camera::recalculatePosition(const glm::vec3 &newPosition) {
  // std::cout << "Recalc position" << std::endl;
  Position[0] = newPosition.x;
  Position[1] = newPosition.y;
  Position[2] = newPosition.z;
}

void Camera::recalculateRotation(const glm::vec3 &newFrontDirection,
                                 const glm::vec2 &newLastRotation) {
  // std::cout << "Recalc rotation" << std::endl;
  Front[0] = newFrontDirection.x;
  Front[1] = newFrontDirection.y;
  Front[2] = newFrontDirection.z;
  LastMousePosition[0] = newLastRotation.x;
  LastMousePosition[1] = newLastRotation.y;
}
