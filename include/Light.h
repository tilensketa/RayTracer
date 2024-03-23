#pragma once

#include <glm/glm.hpp>
#include <string>

enum LightType { Point = 0, Directional };

struct Light {
  int mIndex;
  std::string mName;
  LightType mType = LightType::Point;
  float mIntensity = 1.0f; // Constant
  float mPitch = 0.0f;     // Linear
  float mYaw = 0.0f;       // Quadratic
  glm::vec3 mPosition = glm::vec3(0);
  glm::vec3 mColor = glm::vec3(1);

  Light(const LightType type) {
    mType = type;
    if (type == LightType::Directional) {
      mYaw = 270.0f;
    } else if (type == LightType::Point) {
      mIntensity = 0.5f;
      mPitch = 0.5f;
      mYaw = 0.5f;
    }
  }

  void setName() {
    if (mType == LightType::Point)
      mName = "Point_" + std::to_string(mIndex);
    else if (mType == LightType::Directional)
      mName = "Directional_" + std::to_string(mIndex);
  }
};
