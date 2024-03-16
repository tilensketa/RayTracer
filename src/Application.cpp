#include <glad/glad.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "Application.h"

#include <cassert>
#include <iostream>
#include <memory>

#define MODELS "../models/"
#define SHADERS "../shaders/"

Application::Application(unsigned int width, unsigned int height)
    : mWindow(initWindow(width, height)) {
  mCamera = std::make_unique<Camera>(width, height, 45.0f);
  mQuad = std::make_unique<Quad>();
  mData = std::make_unique<Data>();
  mDataUBO = std::make_unique<UBO>();
  mScene = std::make_shared<Scene>();
  mShader =
      std::make_unique<Shader>(SHADERS "shader.vert", SHADERS "shader.frag");
  mSettings = std::make_shared<Settings>();

  Model monkey(MODELS "monkey.obj");
  mScene->add(monkey);

  mSceneEditor = std::make_shared<SceneEditor>(MODELS, mScene, mSettings);
  /* {
    Model monkey1(MODELS "monkeyLeft.obj");
    mScene->add(monkey1);
  } */

  mData->update(*mCamera, *mScene, *mSettings);
  mDataUBO->init(*mData);

  mTimeStep = 0.0f;
  initCallbacks();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(mWindow.get(), true);
  ImGui_ImplOpenGL3_Init("#version 430 core");
}

Application::~Application() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
}

void Application::run() {
  int a = 0;
  double fps = 0;
  while (!glfwWindowShouldClose(mWindow.get())) {

    mFrameStart = std::chrono::high_resolution_clock::now();

    processInput();
    if (mCamera->update(mWindow.get(), mTimeStep)) {
      mData->updateCamera(*mCamera, *mSettings);
      mDataUBO->update(*mData);
    }

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    mShader->use();

    mDataUBO->bind();
    mQuad->draw();
    mDataUBO->unbind();

    if (mSceneEditor->render(fps, mData->getFloatDataSize())) {
      mData->update(*mCamera, *mScene, *mSettings);
      mDataUBO->update(*mData);
    }

    glfwSwapBuffers(mWindow.get());
    glfwPollEvents();

    mFrameEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> frameDuration = mFrameEnd - mFrameStart;
    mTimeStep = frameDuration.count();
    fps = 1.0 / mTimeStep;
  }
}

std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)>
Application::initWindow(unsigned int width, unsigned int height) {
  if (!glfwInit()) {
    std::cout << "Failed to initialize GLFW" << std::endl;
    assert(false);
  }
  std::cout << "Successfully initialized GLFW" << std::endl;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

  GLFWwindow *window = glfwCreateWindow(width, height, "RayTracer", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    assert(false);
  }
  std::cout << "Successfully created GLFW window" << std::endl;
  glfwMakeContextCurrent(window);
  // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    assert(false);
  }
  glViewport(0, 0, width, height);
  std::cout << "Successfully initialized GLAD" << std::endl;
  return std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)>(
      window, glfwDestroyWindow);
}

void Application::processInput() {
  if (glfwGetKey(mWindow.get(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(mWindow.get(), true);
  if (glfwGetKey(mWindow.get(), GLFW_KEY_R) == GLFW_PRESS)
    saveImage("../renders/hello.png", mCamera->getResolution().x,
              mCamera->getResolution().y);
}

void Application::initCallbacks() {
  glfwSetWindowUserPointer(mWindow.get(), this);
  glfwSetWindowSizeCallback(mWindow.get(), framebuffer_size_callback);
}

void Application::framebuffer_size_callback(GLFWwindow *window, int width,
                                            int height) {
  auto app = static_cast<Application *>(glfwGetWindowUserPointer(window));
  if (app) {
    glViewport(0, 0, width, height);
    app->mCamera->setResolution(width, height);
    app->mData->updateCamera(*(app->mCamera), *(app->mSettings));
    app->mDataUBO->update(*(app->mData));
  }
}

void Application::saveImage(const std::string &filename, int width,
                            int height) {
  std::vector<unsigned char> pixels(width * height * 4);
  glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
  stbi_flip_vertically_on_write(1);
  stbi_write_png(filename.c_str(), width, height, 4, pixels.data(), 0);
}
