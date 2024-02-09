#pragma once

#include <string>

class Shader {
public:
  Shader(const char *vertexShaderPath, const char *fragmentShaderPath);
  void use();
  const unsigned int getID() const { return mID; }

private:
  std::string readShaderFile(const char *filePath);

private:
  unsigned int mID;
};
