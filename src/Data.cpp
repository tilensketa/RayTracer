#include "Data.h"
#include <iostream>

#define BVH_OFFSET 0
#define MATERIAL_OFFSET 1

#define REAL_SETTINGS_OFFSET 10
#define REAL_CAMERA_OFFSET 20
#define REAL_LIGHTS_OFFSET 40
#define REAL_VERTICES_OFFSET 140

void Data::updateCamera(const Camera &camera) {
  mOffset = REAL_CAMERA_OFFSET;
  add(camera.getFOV());
  add(camera.getAspectRatio());
  add(camera.getResolution());
  add(camera.getPosition());
  add(camera.getMatrix());
}

void Data::updateBVH(const Scene &scene, const Settings &settings) {
  // Add vertices
  mOffset = REAL_VERTICES_OFFSET;
  int verticesCount = scene.getVerticesCount();
  for (const Vertex &vertex : scene.getVertices()) {
    add(vertex);
  }

  // Add bvh nodes and triangles
  BVHNode::mIdCounter = -1;
  BVHNode *node =
      BVHNode::buildBVH(scene.getTriangles(), settings.mMaxDepth, 0);
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
  node->clean();
}

void Data::updateLights(const Scene &scene) {
  mOffset = REAL_LIGHTS_OFFSET;
  add(scene.getLightsCount());
  for (const Light &light : scene.getLights()) {
    add(light);
  }
}

void Data::updateSettings(const Settings &settings) {
  mOffset = REAL_SETTINGS_OFFSET;
  add(settings.mDownsampleFactor);
  add(settings.mViewportMode);
}

void Data::updateMaterial(const Scene &scene, bool alone) {
  if (alone)
    mOffset = mData[MATERIAL_OFFSET];
  else
    mData[MATERIAL_OFFSET] = mOffset;
  int materialSum = 0;
  int materialsCount = scene.getMaterialsCount();
  int matOffset = mOffset;
  for (int materialIndex : scene.getMaterialIndexes()) {
    add(matOffset + materialSum + materialsCount);
    materialSum += materialIndex * 3; // 3 -> material size
  }
  for (const Material &material : scene.getMaterials()) {
    add(material);
  }
  mDataFloatSize = mOffset;
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
  mData[mOffset] = (float)value;
  mOffset++;
}

void Data::add(const int &value) {
  mData[mOffset] = (float)value;
  mOffset++;
}

void Data::add(const glm::vec2 &vec) {
  mData[mOffset] = (float)vec.x;
  mData[mOffset + 1] = (float)vec.y;
  mOffset += 2;
}

void Data::add(const glm::ivec2 &vec) {
  mData[mOffset] = static_cast<float>(vec.x);
  mData[mOffset + 1] = static_cast<float>(vec.y);
  mOffset += 2;
}

void Data::add(const glm::vec3 &vec) {
  mData[mOffset] = (float)vec.x;
  mData[mOffset + 1] = (float)vec.y;
  mData[mOffset + 2] = (float)vec.z;
  mOffset += 3;
}

void Data::add(const glm::mat3 &mat) {
  mData[mOffset] = (float)mat[0][0];
  mData[mOffset + 1] = (float)mat[0][1];
  mData[mOffset + 2] = (float)mat[0][2];
  mData[mOffset + 3] = (float)mat[1][0];
  mData[mOffset + 4] = (float)mat[1][1];
  mData[mOffset + 5] = (float)mat[1][2];
  mData[mOffset + 6] = (float)mat[2][0];
  mData[mOffset + 7] = (float)mat[2][1];
  mData[mOffset + 8] = (float)mat[2][2];
  mOffset += 9;
}

void Data::add(const Vertex &vertex) { add(vertex.mModedPosition); }

void Data::add(const Triangle &triangle) {
  add(triangle.mModelIndex);
  add(triangle.mMeshIndex);
  for (int i = 0; i < 3; i++) {
    int indice = triangle.mModedIndices[i];
    add(indice);
  }
  add(triangle.mVertices[0].mNormal);
}

void Data::add(const Material &material) {
  add(material.getDiffuse());
  // add(material.getAmbient());
}

void Data::add(const Light &light) {
  add(light.mType);
  add(light.mIntensity);
  add(light.mPitch);
  add(light.mYaw);
  add(light.mPosition);
  add(light.mColor);
}
