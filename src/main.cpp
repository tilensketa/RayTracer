#include <glad/glad.h>

#include "Model.h"
#include "Quad.h"
#include "Shader.h"
#include "UBO.h"

#include <GLFW/glfw3.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <cassert>
#include <chrono>
#include <iostream>

#define MODELS "../models/"
#define SHADERS "../shaders/"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
GLFWwindow *init(unsigned int width, unsigned int height);
void saveImage(const std::string &filename, int width, int height);

int screenWidth = 1200;
int screenHeight = 800;

Camera camera(screenWidth, screenHeight, 45.0f);
Data data;
UBO dataUBO;

int main() {
  GLFWwindow *window = init(screenWidth, screenHeight);
  Quad quad;

  Scene scene;
  Model monkey(MODELS "monkey.obj");
  scene.add(monkey);
  /* Model monkeyLeft(MODELS "monkeyLeft.obj");
  scene.add(monkeyLeft);
  Model monkeyRight(MODELS "monkeyRight.obj");
  scene.add(monkeyRight); */

  data.updateCamera(camera);
  data.updateScene(scene, 15);

  dataUBO.init(data);
  Shader shader(SHADERS "shader.vert", SHADERS "shader.frag");

  std::chrono::time_point<std::chrono::high_resolution_clock> frameStart,
      frameEnd;

  float ts = 0;
  while (!glfwWindowShouldClose(window)) {

    frameStart = std::chrono::high_resolution_clock::now();

    processInput(window);
    if (camera.update(window, ts)) {
      data.updateCamera(camera);
      dataUBO.update(data);
    }

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shader.use();

    dataUBO.bind();
    quad.draw();
    dataUBO.unbind();

    glfwSwapBuffers(window);
    glfwPollEvents();

    frameEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> frameDuration = frameEnd - frameStart;
    ts = frameDuration.count();
    double fps = 1.0 / frameDuration.count();
    std::cout << "FPS: " << fps << std::endl;
  }

  quad.clean();
  dataUBO.clean();

  glfwTerminate();
  return 0;
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    saveImage("../renders/hello.png", camera.getResolution().x,
              camera.getResolution().y);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  screenWidth = width;
  screenHeight = height;
  camera.setResolution(width, height);
  data.updateCamera(camera);
  dataUBO.update(data);
}

GLFWwindow *init(unsigned int width, unsigned int height) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

  GLFWwindow *window = glfwCreateWindow(width, height, "RayTracer", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    assert(false);
  }
  // std::cout << "GLFW window created" << std::endl;
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    assert(false);
  }
  // std::cout << "GLAD initialized" << std::endl;
  return window;
}

void saveImage(const std::string &filename, int width, int height) {
  std::vector<unsigned char> pixels(width * height * 4);
  glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
  stbi_flip_vertically_on_write(1);
  stbi_write_png(filename.c_str(), width, height, 4, pixels.data(), 0);
}
