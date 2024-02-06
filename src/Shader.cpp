#include "Shader.h"

#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <sstream>

Shader::Shader(const char *vertexShaderPath, const char *fragmentShaderPath) {
  std::string vertexShaderCode = readShaderFile(vertexShaderPath);
  const char *vertexShaderSource = vertexShaderCode.c_str();
  std::string fragmentShaderCode = readShaderFile(fragmentShaderPath);
  const char *fragmentShaderSource = fragmentShaderCode.c_str();

  // Compile Vertex Shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  // Check for compilation errors
  GLint success;
  GLchar infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "Vertex Shader Compilation Failed:\n" << infoLog << std::endl;
  }

  // Compile Fragment Shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  // Check for compilation errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "Fragment Shader Compilation Failed:\n"
              << infoLog << std::endl;
  }

  // Link Shaders into Shader Program
  ID = glCreateProgram();
  glAttachShader(ID, vertexShader);
  glAttachShader(ID, fragmentShader);
  glLinkProgram(ID);

  // Check for linking errors
  glGetProgramiv(ID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(ID, 512, NULL, infoLog);
    std::cout << "Shader Program Linking Failed:\n" << infoLog << std::endl;
  }

  // Delete the shaders as they're linked into our program now and no longer
  // needed
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

std::string Shader::readShaderFile(const char *filePath) {
  std::cout << "Reading file: " << filePath << std::endl;
  std::ifstream file(filePath);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << filePath << std::endl;
    return "";
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  file.close();

  return buffer.str();
}

void Shader::use() {
  glUseProgram(ID);
}
