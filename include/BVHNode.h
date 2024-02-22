#pragma once

#include <Mesh.h>
#include <glm/glm.hpp>

class BVHNode {
public:
  BVHNode() = default;
  static BVHNode *buildBVH(const std::vector<Triangle> &triangles, int maxDepth,
                           int depth);
  static BVHNode *createLeafNode(const std::vector<Triangle> &triangles);

  static std::vector<int> calculateNodeSizes(const BVHNode *node);
  static int calculateNodeSize(const BVHNode *node);

  const int getSplitNumber() const { return mSplitNumber; }
  const int getID() const { return mID; }
  const glm::vec3 &getMaxVert() const { return mMaxVert; }
  const glm::vec3 &getMinVert() const { return mMinVert; }
  BVHNode *getLeft() { return mLeft; }
  BVHNode *getRight() { return mRight; }
  const bool isLeaf() const { return mIsLeaf; }
  const std::vector<Triangle> &getTriangles() const { return mTriangles; }
  const int getTriangleCount() const { return mTriangles.size(); }
  const int getLeftID() const { return mLeftID; }
  const int getRightID() const { return mRightID; }

private:
  void computeBoundingBox();

private:
  static int mSplitNumber;
  static int mIdCounter;

  int mID;
  int mLeftID;
  int mRightID;
  BVHNode *mLeft;
  BVHNode *mRight;
  glm::vec3 mMaxVert;
  glm::vec3 mMinVert;
  std::vector<Triangle> mTriangles;
  bool mIsLeaf;
};
