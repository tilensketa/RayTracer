#pragma once

#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>
#include <string>

class Model {
public:
  Model() = default;
  Model(const std::string objPath);
  ~Model(){std::cout << "Model dest: " << mName << std::endl;}

  // Name
  const std::string &getName() const { return mName; }

  // Position
  const glm::vec3 &getPosition() const { return mPosition; }
  glm::vec3 &getPosition() { return mPosition; }
  glm::vec3 &modPosition() { return mPosition; }
  void setPosition(const glm::vec3 &newPosition) { mPosition = newPosition; }
  void updatePosition();

  // Index
  void setIndex(int id);
  const int getIndex() const { return mIndex; }

  // Meshes
  const std::vector<Mesh> &getMeshes() const { return mMeshes; }
  const int getMeshCount() const { return mMeshes.size(); }
  std::vector<Mesh> &modMeshes() { return mMeshes; }

  // Bounding box
  const glm::vec3 &getMaxVert() const { return mMaxVert; }
  const glm::vec3 &getMinVert() const { return mMinVert; }

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
