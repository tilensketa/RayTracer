#pragma once

#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <string>

class Model {
public:
  Model() = default;
  Model(const std::string &objPath);

  void setIndex(int id);

  const int getIndex() const { return mIndex; }
  const std::vector<Mesh> &getMeshes() const { return mMeshes; }
  const int getMeshCount() const { return mMeshes.size(); }
  const glm::vec3 &getMaxVert() const { return mMaxVert; }
  const glm::vec3 &getMinVert() const { return mMinVert; }

private:
  void processNode(const aiNode *node, const aiScene *scene);
  Material processNodeMaterial(const aiMaterial *material);
  void createBoundingBox();

private:
  int mIndex;
  std::vector<Mesh> mMeshes;
  glm::vec3 mMaxVert;
  glm::vec3 mMinVert;
};
