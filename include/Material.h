#pragma once

#include <glm/glm.hpp>

class Material {
public:
  Material() = default;

  // Diffuse color
  void setDiffuse(const glm::vec3 &diffuse) { mDiffuse = diffuse; }
  const glm::vec3 &getDiffuse() const { return mDiffuse; }
  glm::vec3 &modDiffuse() { return mDiffuse; }

  // Ambient color
  void setAmbient(const glm::vec3 &ambient) { mAmbient = ambient; }
  const glm::vec3 &getAmbient() const { return mAmbient; }
  glm::vec3 &modAmbient() { return mAmbient; }

private:
  glm::vec3 mDiffuse;
  glm::vec3 mAmbient;
};
