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
  int mModedIndices[3];
  glm::vec3 mCenter;
  int mModelIndex;
  int mMeshIndex;

  void recalculateCenter() {
    mCenter = glm::vec3(0.0f);
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        mCenter[j] += mVertices[i].mPosition[j];
      }
    }
    mCenter /= 3.0f;
  }
};

class Mesh {
public:
  Mesh() = default;
  Mesh(std::vector<Vertex> vertices, std::vector<int> indices);

  // Index
  void setIndex(const int id);
  const int getIndex() const { return mIndex; }

  // Triangles
  const int getTriangleCount() const { return mTriangles.size(); }
  const std::vector<Triangle> &getTriangles() const { return mTriangles; }
  std::vector<Triangle> &modTriangles() { return mTriangles; }

  // Vertices
  const int getVerticesCount() const { return mVertices.size(); }
  const std::vector<Vertex> &getVertices() const { return mVertices; }

  // Material
  void setMaterial(const Material &material) { mMaterial = material; }
  const Material &getMaterial() const { return mMaterial; }
  Material &modMaterial() { return mMaterial; }

  // Bounding box
  const glm::vec3 &getMaxVert() const { return mMaxVert; }
  const glm::vec3 &getMinVert() const { return mMinVert; }

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
