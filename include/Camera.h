#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Camera {
public:
  Camera() = default;
  Camera(unsigned int width, unsigned int height, float fov);

  bool update(GLFWwindow *window, float ts);

  void setWidth(unsigned int width) { Width = width; }
  void setHeight(unsigned int height) { Height = height; }

private:
  void recalculatePosition(const glm::vec3 &newPosition);
  void recalculateRotation(const glm::vec3 &newFrontDirection,
                           const glm::vec2 &newLastRotation);

private:
  unsigned int Width;
  unsigned int Height;
  float FOV;
  float Position[3];
  float Front[3];

  float MoveSpeed = 5;
  float RotateSpeed = 1;
  float LastMousePosition[2] = {0, 0};
};
