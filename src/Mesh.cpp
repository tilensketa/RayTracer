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
  for (int j = 0; j < getTriangleCount(); j++) {
    const Triangle &triangle = mTriangles[j];
    for (int k = 0; k < 3; k++) {
      const Vertex &vert = triangle.mVertices[k];
      for (int i = 0; i < 3; i++) {
        minVert[i] = glm::min(minVert[i], vert.mModedPosition[i]);
        maxVert[i] = glm::max(maxVert[i], vert.mModedPosition[i]);
      }
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

void Mesh::updatePosition() {
  for (Vertex &vertex : mVertices) {
    vertex.mModedPosition = vertex.mPosition + mPosition;
  }
  for (Triangle &triangle : mTriangles) {
    for (int i = 0; i < 3; i++) {
      triangle.mVertices[i].mModedPosition =
          triangle.mVertices[i].mPosition + mPosition;
    }
    triangle.recalculateCenter();
  }
  createBoundingBox();
}
