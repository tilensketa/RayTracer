#include "Data.h"
#include <iostream>

#define CAMERA_OFFSET 0
#define VERTICES_OFFSET 1
#define BVH_OFFSET 2
#define MATERIAL_OFFSET 3

void Data::updateCamera(const Camera &camera) {
  mOffset = 10;
  mData[CAMERA_OFFSET] = mOffset;
  add(camera.getConfig());
  add(camera.getFOV());
  add(camera.getAspectRatio());
  add(camera.getResolution());
  add(camera.getPosition());
  add(camera.getMatrix());
}

void Data::updateScene(Scene &scene, int maxDepth) {
  std::vector<Triangle> triangles;
  std::vector<Vertex> vertices;
  std::vector<Material> materials;
  std::vector<int> materialIndexes;

  int indicesOffset = 0;
  for (Model &model : scene.modModels()) {
    materialIndexes.push_back(model.getMeshCount());
    for (Mesh &mesh : model.modMeshes()) {
      const Material material = mesh.getMaterial();
      materials.push_back(material);
      for (Triangle &triangle : mesh.modTriangles()) {
        triangle.mIndices[0] += indicesOffset;
        triangle.mIndices[1] += indicesOffset;
        triangle.mIndices[2] += indicesOffset;
        triangles.push_back(triangle);
      }
      for (const Vertex &vertex : mesh.getVertices()) {
        vertices.push_back(vertex);
      }
      indicesOffset += mesh.getVerticesCount();
    }
  }

  // Add vertices
  mData[VERTICES_OFFSET] = mOffset;
  int verticesCount = vertices.size();
  for (const Vertex &vertex : vertices) {
    add(vertex);
  }

  // Add bvh nodes and triangles
  BVHNode *node = BVHNode::buildBVH(triangles, maxDepth, 0);
  std::vector<int> sizes = BVHNode::calculateNodeSizes(node);
  int numberOfNodes = sizes.size();

  mData[BVH_OFFSET] = mOffset;
  int bvhNodesSum = 0;
  int bvhOffset = mOffset;
  for (int size : sizes) {
    add(bvhOffset + bvhNodesSum + numberOfNodes);
    bvhNodesSum += size;
  }
  updateNode(node);

  // Add materials
  mData[MATERIAL_OFFSET] = mOffset;
  int materialSum = 0;
  int materialsCount = materialIndexes.size();
  int matOffset = mOffset;
  for (int materialIndex : materialIndexes) {
    add(matOffset + materialSum + materialsCount);
    materialSum += materialIndex * 6; // 6 -> material size
  }
  for (const Material &material : materials) {
    add(material);
  }

  std::cout << mOffset << std::endl;
}

void Data::updateNode(BVHNode *node) {
  add(node->getMaxVert());
  add(node->getMinVert());
  add(node->isLeaf());
  if (node->isLeaf()) {
    updateLeafNode(node);
    return;
  }
  add(node->getLeftID());
  add(node->getRightID());

  updateNode(node->getLeft());
  updateNode(node->getRight());
}

void Data::updateLeafNode(BVHNode *node) {
  add(node->getTriangleCount());
  for (const Triangle &triangle : node->getTriangles()) {
    add(triangle);
  }
}

void Data::add(const bool bol) {
  mData[mOffset] = (float)bol;
  mOffset++;
}

void Data::add(const float &value) {
  mData[mOffset] = value;
  mOffset++;
}

void Data::add(const int &value) {
  mData[mOffset] = (float)value;
  mOffset++;
}

void Data::add(const glm::vec2 &vec) {
  mData[mOffset] = vec.x;
  mData[mOffset + 1] = vec.y;
  mOffset += 2;
}

void Data::add(const glm::ivec2 &vec) {
  mData[mOffset] = static_cast<float>(vec.x);
  mData[mOffset + 1] = static_cast<float>(vec.y);
  mOffset += 2;
}

void Data::add(const glm::vec3 &vec) {
  mData[mOffset] = vec.x;
  mData[mOffset + 1] = vec.y;
  mData[mOffset + 2] = vec.z;
  mOffset += 3;
}

void Data::add(const glm::mat3 &mat) {
  mData[mOffset] = mat[0][0];
  mData[mOffset + 1] = mat[0][1];
  mData[mOffset + 2] = mat[0][2];
  mData[mOffset + 3] = mat[1][0];
  mData[mOffset + 4] = mat[1][1];
  mData[mOffset + 5] = mat[1][2];
  mData[mOffset + 6] = mat[2][0];
  mData[mOffset + 7] = mat[2][1];
  mData[mOffset + 8] = mat[2][2];
  mOffset += 9;
}

void Data::add(const Vertex &vertex) { add(vertex.mPosition); }

void Data::add(const Triangle &triangle) {
  add(triangle.mModelIndex);
  add(triangle.mMeshIndex);
  for (int i = 0; i < 3; i++) {
    int indice = triangle.mIndices[i];
    add(indice);
  }
  add(triangle.mVertices[0].mNormal);
}

void Data::add(const Material &material) {
  add(material.getDiffuse());
  add(material.getAmbient());
}
