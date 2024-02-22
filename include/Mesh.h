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
  int mIndices[3];
  glm::vec3 mCenter;
  int mModelIndex;
  int mMeshIndex;
};

class Mesh {
public:
  Mesh() = default;
  Mesh(std::vector<Vertex> vertices, std::vector<int> indices);

  void setIndex(const int id);
  void setMaterial(const Material &material) { mMaterial = material; }

  const int getIndex() const { return mIndex; }
  const int getTriangleCount() const { return mTriangles.size(); }
  const std::vector<Triangle> &getTriangles() const { return mTriangles; }
  const int getVerticesCount() const { return mVertices.size(); }
  const std::vector<Vertex> &getVertices() const { return mVertices; }
  const Material &getMaterial() const { return mMaterial; }
  const glm::vec3 &getMaxVert() const { return mMaxVert; }
  const glm::vec3 &getMinVert() const { return mMinVert; }

  std::vector<Triangle> &modTriangles() { return mTriangles; }

private:
  void createBoundingBox();

private:
  int mIndex;
  std::vector<Triangle> mTriangles;
  std::vector<Vertex> mVertices;
  Material mMaterial;
  glm::vec3 mMaxVert;
  glm::vec3 mMinVert;
};
