#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Material.h"

struct Vertex {
  glm::vec3 mPosition;
  glm::vec3 mNormal;
};

struct Triangle {
  Vertex mVertices[3];
};

class Mesh {
public:
  Mesh() = default;
  Mesh(std::vector<Vertex> vertices, std::vector<int> indices);

  void setMaterial(const Material &material) { mMaterial = material; }

  const int getTriangleCount() const { return mTriangles.size(); }
  const std::vector<Triangle> &getTriangles() const { return mTriangles; }
  const Material &getMaterial() const { return mMaterial; }
  const glm::vec3 &getMaxVert() const { return mMaxVert; }
  const glm::vec3 &getMinVert() const { return mMinVert; }

private:
  void createBoundingBox();

private:
  std::vector<Triangle> mTriangles;
  Material mMaterial;
  glm::vec3 mMaxVert;
  glm::vec3 mMinVert;
};
