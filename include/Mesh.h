#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

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

  const int getTriangleCount() const { return mTriangles.size(); }
  const std::vector<Triangle> &getTriangles() const { return mTriangles; }

private:
  std::vector<Triangle> mTriangles;
};
