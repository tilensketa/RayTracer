#pragma once

#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <string>

class Model {
public:
  Model() = default;
  Model(const std::string objPath);

  const std::string &getName() const { return mName; }

  void setIndex(int id);
  const int getIndex() const { return mIndex; }

  const std::vector<Mesh> &getMeshes() const { return mMeshes; }
  const int getMeshCount() const { return mMeshes.size(); }
  std::vector<Mesh> &modMeshes() { return mMeshes; }

  const glm::vec3 &getMaxVert() const { return mMaxVert; }
  const glm::vec3 &getMinVert() const { return mMinVert; }

  const glm::vec3 &getPosition() const { return mPosition; }
  glm::vec3 &modPosition() { return mPosition; }

private:
  void processNode(const aiNode *node, const aiScene *scene);
  Material processNodeMaterial(const aiMaterial *material);
  void createBoundingBox();

private:
  std::string mName;
  glm::vec3 mPosition = glm::vec3(0.0f);
  int mIndex;
  std::vector<Mesh> mMeshes;
  glm::vec3 mMaxVert;
  glm::vec3 mMinVert;
};
