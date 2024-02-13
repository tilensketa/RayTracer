#include "Data.h"
#include <iostream>

void Data::update(const Camera &camera) {
  mData[0] = 18;
  mData[1] = camera.getConfig();
  mData[2] = camera.getFOV();
  mData[3] = camera.getAspectRatio();
  mData[4] = static_cast<float>(camera.getResolution().x);
  mData[5] = static_cast<float>(camera.getResolution().y);
  mData[6] = camera.getPosition().x;
  mData[7] = camera.getPosition().y;
  mData[8] = camera.getPosition().z;
  mData[9] = camera.getMatrix()[0][0];
  mData[10] = camera.getMatrix()[0][1];
  mData[11] = camera.getMatrix()[0][2];
  mData[12] = camera.getMatrix()[1][0];
  mData[13] = camera.getMatrix()[1][1];
  mData[14] = camera.getMatrix()[1][2];
  mData[15] = camera.getMatrix()[2][0];
  mData[16] = camera.getMatrix()[2][1];
  mData[17] = camera.getMatrix()[2][2];
}

void Data::update(const Scene &scene) {
  mOffset = (int)mData[0];

  int modelCount = scene.getModelCount();
  add(modelCount);
  for (int i = 0; i < modelCount; i++) {
    const Model &model = scene.getModels()[i];
    add(model.getIndex());
    add(model.getMaxVert());
    add(model.getMinVert());
    int meshCount = model.getMeshCount();
    add(meshCount);
    for (int j = 0; j < meshCount; j++) {
      const Mesh &mesh = model.getMeshes()[j];
      add(mesh.getMaxVert());
      add(mesh.getMinVert());
      const Material &material = mesh.getMaterial();
      add(material.getDiffuse());
      add(material.getAmbient());
      int triangleCount = mesh.getTriangleCount();
      add(triangleCount);
      for (int k = 0; k < triangleCount; k++) {
        const Triangle &triangle = mesh.getTriangles()[k];
        for (int l = 0; l < 3; l++) {
          const Vertex &vert = triangle.mVertices[l];
          add(vert.mPosition);
        }
        add(triangle.mVertices[0].mNormal);
      }
    }
  }
}
void printID(BVHNode *node) {
  std::cout << node->getID() << " -> ";
  if (node->getLeft() == nullptr) {
    std::cout << "-1 ";
  } else {
    std::cout << node->getLeftID() << " ";
  }
  if (node->getRight() == nullptr) {
    std::cout << "-1 ";
  } else {
    std::cout << node->getRightID() << " ";
  }
  std::cout << std::endl;
  if (node->getRight() == nullptr || node->getLeft() == nullptr)
    return;
  printID(node->getLeft());
  printID(node->getRight());
}

void Data::updateBVH(const Scene &scene, int maxTrianglesInNode) {
  std::vector<Triangle> triangles;
  for (const Model &model : scene.getModels()) {
    for (const Mesh &mesh : model.getMeshes()) {
      for (const Triangle &triangle : mesh.getTriangles()) {
        triangles.push_back(triangle);
      }
    }
  }
  BVHNode *node = BVHNode::buildBVH(triangles, maxTrianglesInNode);
  /* std::cout << "--------------" << std::endl;
  printID(node);
  std::cout << "--------------" << std::endl; */
  std::vector<int> sizes = BVHNode::calculateNodeSizes(node);
  int numberOfNodes = sizes.size();
  mOffset = (int)mData[0];
  // add(numberOfNodes);
  int sum = 0;
  for (int size : sizes) {
    add(mOffset + sum + numberOfNodes);
    sum += size;
  }
  updateNode(node);
  /* for (int i = 0; i < 10000; i++) {
    std::cout << mData[i] << std::endl;
  } */
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
    add(triangle.mModelIndex);
    add(triangle.mMeshIndex);
    for (int i = 0; i < 3; i++) {
      const Vertex &vertex = triangle.mVertices[i];
      add(vertex.mPosition);
    }
    add(triangle.mVertices[0].mNormal);
  }
}

void Data::add(const glm::vec3 &vec) {
  mData[mOffset] = vec.x;
  mData[mOffset + 1] = vec.y;
  mData[mOffset + 2] = vec.z;
  mOffset += 3;
}

void Data::add(const float &value) {
  mData[mOffset] = value;
  mOffset++;
}

void Data::add(const int &value) {
  mData[mOffset] = (float)value;
  mOffset++;
}

void Data::add(const bool bol) {
  mData[mOffset] = (float)bol;
  mOffset++;
}
