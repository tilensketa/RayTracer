#pragma once

#include <glm/glm.hpp>

class Material {
public:
  Material() = default;

  void setDiffuse(const glm::vec3 &diffuse) { mDiffuse = diffuse; }
  void setAmbient(const glm::vec3 &ambient) { mAmbient = ambient; }

  const glm::vec3 &getDiffuse() const { return mDiffuse; }
  const glm::vec3 &getAmbient() const { return mAmbient; }

private:
  glm::vec3 mDiffuse;
  glm::vec3 mAmbient;
};