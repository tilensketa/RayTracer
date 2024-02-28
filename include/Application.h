#pragma once

#include "Quad.h"
#include "Shader.h"
#include "UBO.h"

#include <GLFW/glfw3.h>

#include <chrono>

class Application {
public:
  Application(unsigned int width, unsigned int height);
  ~Application();

  void run();

private:
  GLFWwindow *initWindow(unsigned int width, unsigned int height);
  void processInput();
  void saveImage(const std::string &filename, int width, int height);

  static void framebuffer_size_callback(GLFWwindow *window, int width,
                                        int height);
  void initCallbacks();

private:
  int mScreenWidth;
  int mScreenHeight;
  GLFWwindow *mWindow;
  Camera *mCamera;
  Data *mData;
  UBO *mDataUBO;
  Quad *mQuad;
  Shader *mShader;
  float mTimeStep;
  std::chrono::time_point<std::chrono::high_resolution_clock> mFrameStart,
      mFrameEnd;
};
