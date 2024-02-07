#pragma once

#include <string>

class Shader {
public:
  Shader(const char *vertexShaderPath, const char *fragmentShaderPath);
  void use();

private:
  std::string readShaderFile(const char *filePath);

private:
  unsigned int ID;
};
