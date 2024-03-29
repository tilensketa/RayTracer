#include <glad/glad.h>

#include "GLFW/glfw3.h"

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
  mCamera = std::make_shared<Camera>(width, height, 45.0f);
  mQuad = std::make_unique<Quad>();
  mData = std::make_unique<Data>();
  mDataUBO = std::make_unique<UBO>();
  mScene = std::make_shared<Scene>();
  mShader =
      std::make_unique<Shader>(SHADERS "shader.vert", SHADERS "shader.frag");
  mSettings = std::make_shared<Settings>();

  mScene->addModel(MODELS "Default/Monkey.obj");
  mScene->addLight(LightType::Directional);

  mSceneEditor =
      std::make_shared<SceneEditor>(MODELS, mScene, mCamera, mSettings);

  mData->updateSettings(*mSettings);
  mData->updateCamera(*mCamera);
  mData->updateLights(*mScene);
  mData->updateBVH(*mScene, *mSettings);
  mData->updateMaterial(*mScene, false);
  mDataUBO->init(*mData);

  mTimeStep = 0.0f;
  initCallbacks();

  setupImGui();
}

Application::~Application() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
}

void Application::setupImGui() {

  ImGui::CreateContext();

  ImGuiStyle &style = ImGui::GetStyle();
  ImVec4 *colors = style.Colors;
  colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
  colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
  colors[ImGuiCol_Header] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
  colors[ImGuiCol_HeaderHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
  colors[ImGuiCol_HeaderActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
  colors[ImGuiCol_TitleBgActive] = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
  colors[ImGuiCol_Button] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
  colors[ImGuiCol_ButtonHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
  colors[ImGuiCol_ButtonActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
  colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
  colors[ImGuiCol_FrameBgActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
  colors[ImGuiCol_ScrollbarBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
  colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
  colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
  colors[ImGuiCol_CheckMark] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
  colors[ImGuiCol_SliderGrab] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
  colors[ImGuiCol_SliderGrabActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
  colors[ImGuiCol_Separator] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
  colors[ImGuiCol_SeparatorHovered] = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
  colors[ImGuiCol_SeparatorActive] = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
  colors[ImGuiCol_TextSelectedBg] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
  colors[ImGuiCol_ResizeGripHovered] =ImVec4(0.5f, 0.5f, 0.5f, 1.0f); 
  colors[ImGuiCol_ResizeGrip] =ImVec4(0.4f, 0.4f, 0.4f, 1.0f); 
  colors[ImGuiCol_ResizeGripActive] =ImVec4(0.4f, 0.4f, 0.4f, 1.0f); 

  ImGui_ImplGlfw_InitForOpenGL(mWindow.get(), true);
  ImGui_ImplOpenGL3_Init("#version 430 core");
}

void Application::run() {
  int a = 0;
  double fps = 0;
  while (!glfwWindowShouldClose(mWindow.get())) {

    mFrameStart = std::chrono::high_resolution_clock::now();

    processInput();
    if (mCamera->update(mWindow.get(), mTimeStep)) {
      mData->updateCamera(*mCamera);
      mDataUBO->update(*mData);
    }

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    mShader->use();

    mDataUBO->bind();
    mQuad->draw();
    mDataUBO->unbind();

    if (mShowEditor) {
      ChangeType change = mSceneEditor->render(fps, mData->getFloatDataSize());
      if (change == ChangeType::BVHType) {
        mData->updateBVH(*mScene, *mSettings);
        mData->updateMaterial(*mScene, false);
      }
      if (change == ChangeType::MaterialType)
        mData->updateMaterial(*mScene, true);
      if (change == ChangeType::CameraType)
        mData->updateCamera(*mCamera);
      if (change == ChangeType::SettingsType)
        mData->updateSettings(*mSettings);
      if (change == ChangeType::LightType)
        mData->updateLights(*mScene);

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
  // Enable full screen mode
  /* glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, 1920, 1080,
                       GLFW_DONT_CARE); */
  // Maximize the window
  glfwMaximizeWindow(window);
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
  if (glfwGetKey(mWindow.get(), GLFW_KEY_N) == GLFW_PRESS) {
    double currentTime = glfwGetTime();
    if (currentTime - mEditorToggleTimer > 0.3) {
      mShowEditor = !mShowEditor;
      mEditorToggleTimer = currentTime;
    }
  }
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
    app->mData->updateCamera(*(app->mCamera));
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
