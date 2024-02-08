#pragma once

#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <string>

class Model {
public:
  Model() = default;
  Model(std::string objPath);

  void setIndex(int id) { mIndex = id; }
  const int getIndex() const { return mIndex; }
  const std::vector<Mesh> &getMeshes() const { return mMeshes; }
  const int getMeshCount() const { return mMeshes.size(); }

private:
  void ProcessNode(const aiNode *node, const aiScene *scene);

private:
  int mIndex;
  std::vector<Mesh> mMeshes;
};
