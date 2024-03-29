#pragma once

#include "Quad.h"
#include "SceneEditor.h"
#include "Shader.h"
#include "UBO.h"

#include <GLFW/glfw3.h>

#include <chrono>
#include <memory>

class Application {
public:
  Application(unsigned int width, unsigned int height);
  ~Application();

  void run();

private:
  std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)>
  initWindow(unsigned int width, unsigned int height);
  void setupImGui();
  void processInput();
  void saveImage(const std::string &filename, int width, int height);

  static void framebuffer_size_callback(GLFWwindow *window, int width,
                                        int height);
  void initCallbacks();

private:
  std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> mWindow;
  std::shared_ptr<Camera> mCamera;
  std::unique_ptr<Data> mData;
  std::unique_ptr<UBO> mDataUBO;
  std::unique_ptr<Quad> mQuad;
  std::unique_ptr<Shader> mShader;
  std::shared_ptr<Scene> mScene;
  std::shared_ptr<SceneEditor> mSceneEditor;
  std::shared_ptr<Settings> mSettings;
  float mTimeStep;
  std::chrono::time_point<std::chrono::high_resolution_clock> mFrameStart,
      mFrameEnd;
  bool mShowEditor = true;
  double mEditorToggleTimer = 0.0;
};
