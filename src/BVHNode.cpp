#include "BVHNode.h"
#include <iostream>

int BVHNode::mSplitNumber = 0;
int BVHNode::mIdCounter = -1;

BVHNode *BVHNode::createLeafNode(const std::vector<Triangle> &triangles) {
  BVHNode *node = new BVHNode;
  node->mID = mIdCounter;
  node->mLeftID = -1;
  node->mRightID = -1;
  node->mIsLeaf = true;
  node->mTriangles = triangles;
  node->computeBoundingBox();
  node->mLeft = nullptr;
  node->mRight = nullptr;
  return node;
}

BVHNode *BVHNode::buildBVH(const std::vector<Triangle> &triangles, int maxDepth,
                           int depth) {
  mIdCounter++;
  if (depth == maxDepth || triangles.size() <= 10) {
    return createLeafNode(triangles);
  }

  BVHNode *node = new BVHNode;
  node->mID = mIdCounter;
  node->mLeftID = mIdCounter + 1;
  node->mIsLeaf = false;
  node->mTriangles = triangles;
  node->computeBoundingBox();

  glm::vec3 mid = (node->getMaxVert() + node->getMinVert()) / 2.0f;
  glm::vec3 sizeOfAABB = node->getMaxVert() - node->getMinVert();
  float width = sizeOfAABB.x;
  float length = sizeOfAABB.y;
  float height = sizeOfAABB.z;

  int splitCoord = 0;
  if (width > length && width > height)
    splitCoord = 0;
  else if (length > width && length > height)
    splitCoord = 1;
  else if (height > width && height > length)
    splitCoord = 2;

  std::vector<Triangle> leftTriangles;
  std::vector<Triangle> rightTriangles;

  for (const Triangle &triangle : node->mTriangles) {
    if (triangle.mCenter[splitCoord] >= mid[splitCoord]) {
      leftTriangles.push_back(triangle);
    } else {
      rightTriangles.push_back(triangle);
    }
  }

  node->mLeft = buildBVH(leftTriangles, maxDepth, depth + 1);
  node->mRightID = mIdCounter + 1;
  node->mRight = buildBVH(rightTriangles, maxDepth, depth + 1);

  return node;
}

void BVHNode::computeBoundingBox() {
  float max = std::numeric_limits<float>::max();
  glm::vec3 minVert = glm::vec3(max, max, max);
  glm::vec3 maxVert = glm::vec3(-max, -max, -max);
  for (int j = 0; j < mTriangles.size(); j++) {
    const Triangle &triangle = mTriangles[j];
    for (int k = 0; k < 3; k++) {
      const Vertex &vert = triangle.mVertices[k];
      for (int i = 0; i < 3; i++) {
        minVert[i] = glm::min(minVert[i], vert.mPosition[i]);
        maxVert[i] = glm::max(maxVert[i], vert.mPosition[i]);
      }
    }
  }
  mMaxVert = maxVert;
  mMinVert = minVert;
}
int BVHNode::calculateNodeSize(const BVHNode *node) {
  int size = 0;

  size += 6; // aabb
  size += 1; // isLeaf

  if (!node->mIsLeaf) {
    size += 2; // left/right
    return size;
  }
  size += 1; // triangle count
  for (const Triangle &triangle : node->mTriangles) {
    size += 2; // model,mesh index
    size += 3; // indices
    size += 3; // Normal
  }
  return size;
}

std::vector<int> BVHNode::calculateNodeSizes(const BVHNode *node) {
  std::vector<int> sizes;

  if (!node) {
    return sizes;
  }

  sizes.push_back(calculateNodeSize(node));

  std::vector<int> leftSizes = calculateNodeSizes(node->mLeft);
  std::vector<int> rightSizes = calculateNodeSizes(node->mRight);

  sizes.insert(sizes.end(), leftSizes.begin(), leftSizes.end());
  sizes.insert(sizes.end(), rightSizes.begin(), rightSizes.end());

  return sizes;
}
