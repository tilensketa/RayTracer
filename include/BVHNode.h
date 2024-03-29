#pragma once

#include <Mesh.h>
#include <glm/glm.hpp>

class BVHNode {
public:
  ~BVHNode();
  void clean();

  static BVHNode *buildBVH(const std::vector<Triangle> &triangles,
                           const int maxDepth, const int maxTrianglesInLeaf,
                           int depth);
  static BVHNode *createLeafNode(const std::vector<Triangle> &triangles);

  // Node size
  static std::vector<int> calculateNodeSizes(const BVHNode *node);
  static int calculateNodeSize(const BVHNode *node);

  // ID
  const int getID() const { return mID; }
  const int getLeftID() const { return mLeftID; }
  const int getRightID() const { return mRightID; }

  // Node*
  BVHNode *getLeft() { return mLeft; }
  BVHNode *getRight() { return mRight; }

  // Bounding box
  const glm::vec3 &getMaxVert() const { return mMaxVert; }
  const glm::vec3 &getMinVert() const { return mMinVert; }

  // Triangles
  const std::vector<Triangle> &getTriangles() const { return mTriangles; }
  const int getTriangleCount() const { return mTriangles.size(); }

  // Leaf
  const bool isLeaf() const { return mIsLeaf; }

private:
  void computeBoundingBox();

public:
  static int mIdCounter;

private:
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
