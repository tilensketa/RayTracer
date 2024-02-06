#include "Quad.h"
#include "Shader.h"
#include "UBO.h"

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>

#include <chrono>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
GLFWwindow *init(unsigned int width, unsigned int height);

Data data;
UBO dataUBO;

unsigned int screenWidth = 1200;
unsigned int screenHeight = 800;

int main() {

  GLFWwindow *window = init(screenWidth, screenHeight);
  Quad quad;

  Scene scene;
  scene.add({{-1, 0, 0}, {0, 0, 1}, 0.5f});
  scene.add({{0, 0, 0}, {0, 1, 0}, 0.5f});
  scene.add({{1, 0, 0}, {1, 0, 0}, 0.5f});
  scene.add({{0, 0, -1}, {1, 1, 0}, 1.0f});

  data = {screenWidth, screenHeight, scene};
  dataUBO.init(data);
  Shader shader("../shaders/shader.vert", "../shaders/shader.frag");

  std::chrono::time_point<std::chrono::high_resolution_clock> frameStart,
      frameEnd;

  while (!glfwWindowShouldClose(window)) {

    frameStart = std::chrono::high_resolution_clock::now();

    processInput(window);

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
    double fps = 1.0 / frameDuration.count();
    // std::cout << "FPS: " << fps << std::endl;
  }

  quad.clean();
  dataUBO.clean();

  glfwTerminate();
  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
  screenWidth = width;
  screenHeight = height;
  data.ResolutionX = width;
  data.ResolutionY = height;
  dataUBO.update(data);
}

GLFWwindow *init(unsigned int width, unsigned int height) {
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

  // glfw window creation
  // --------------------
  GLFWwindow *window = glfwCreateWindow(width, height, "RayTracer", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return nullptr;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return nullptr;
  }
  return window;
}