#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<int> indices) {
  for (int i = 0; i < indices.size(); i += 3) {
    Triangle triangle;
    triangle.mVertices[0] = vertices[indices[i]];
    triangle.mVertices[1] = vertices[indices[i + 1]];
    triangle.mVertices[2] = vertices[indices[i + 2]];

    mTriangles.push_back(triangle);
  }
}
