#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<int> indices) {
  mVertices = vertices;
  for (int i = 0; i < indices.size(); i += 3) {
    Triangle triangle;
    triangle.mVertices[0] = vertices[indices[i]];
    triangle.mVertices[1] = vertices[indices[i + 1]];
    triangle.mVertices[2] = vertices[indices[i + 2]];

    triangle.mIndices[0] = indices[i];
    triangle.mIndices[1] = indices[i + 1];
    triangle.mIndices[2] = indices[i + 2];

    triangle.recalculateCenter();
    mTriangles.push_back(triangle);
  }
  createBoundingBox();
}

void Mesh::createBoundingBox() {
  float max = std::numeric_limits<float>::max();
  glm::vec3 minVert = glm::vec3(max, max, max);
  glm::vec3 maxVert = glm::vec3(-max, -max, -max);
  for (const Vertex &vertex : mVertices) {
    for (int i = 0; i < 3; i++) {
      minVert[i] = glm::min(minVert[i], vertex.mModedPosition[i]);
      maxVert[i] = glm::max(maxVert[i], vertex.mModedPosition[i]);
    }
  }
  mMaxVert = maxVert;
  mMinVert = minVert;
}

void Mesh::setIndex(const int id) {
  mIndex = id;
  for (Triangle &triangle : mTriangles) {
    triangle.mMeshIndex = mIndex;
  }
}

void Mesh::update() {
  for (Vertex &vertex : mVertices) {
    recalculateVertex(vertex);
  }
  for (Triangle &triangle : mTriangles) {
    for (int i = 0; i < 3; i++) {
      recalculateVertex(triangle.mVertices[i]);
    }
    triangle.recalculateCenter();
  }
  createBoundingBox();
}

void Mesh::recalculateVertex(Vertex &vertex) {
  // Scale
  vertex.mModedPosition = vertex.mPosition * mScale;
  // Position
  vertex.mModedPosition += mPosition;
  // Rotate
  glm::vec3 o = vertex.mModedPosition - mPosition;
  float alpha = glm::radians(mRotation.x);
  float beta = glm::radians(mRotation.y);
  float gamma = glm::radians(mRotation.z);
  glm::vec3 mid1;
  mid1.x = o.x;
  mid1.y = o.y * glm::cos(alpha) - o.z * glm::sin(alpha);
  mid1.z = o.y * glm::sin(alpha) + o.z * glm::cos(alpha);
  glm::vec3 mid2;
  mid2.x = mid1.x * glm::cos(beta) + mid1.z * glm::sin(beta);
  mid2.y = mid1.y;
  mid2.z = -mid1.x * glm::sin(beta) + mid1.z * glm::cos(beta);
  vertex.mModedPosition.x = mid2.x * glm::cos(gamma) - mid2.y * glm::sin(gamma);
  vertex.mModedPosition.y = mid2.x * glm::sin(gamma) + mid2.y * glm::cos(gamma);
  vertex.mModedPosition.z = mid2.z;
  vertex.mModedPosition += mPosition;

}
