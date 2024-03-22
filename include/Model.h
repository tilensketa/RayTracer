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

  // Path
  const std::string &getPath() const { return mPath; }

  // Name
  const std::string getName() const { return mName; }
  void setName(const std::string newName) { mName = newName; }

  // Position
  const glm::vec3 &getPosition() const { return mPosition; }
  glm::vec3 &modPosition() { return mPosition; }

  // Scale
  const glm::vec3 &getScale() const { return mScale; }
  glm::vec3 &modScale() { return mScale; }

  // Rotation
  const glm::vec3 &getRotation() const { return mRotation; }
  glm::vec3 &modRotation() { return mRotation; }

  // Index
  void setIndex(int id) { mIndex = id; }
  const int getIndex() const { return mIndex; }
  void setSceneIndex(int id);

  // Meshes
  const std::vector<Mesh> &getMeshes() const { return mMeshes; }
  const int getMeshCount() const { return mMeshes.size(); }
  std::vector<Mesh> &modMeshes() { return mMeshes; }

  // Bounding box
  const glm::vec3 &getMaxVert() const { return mMaxVert; }
  const glm::vec3 &getMinVert() const { return mMinVert; }

  void update();

private:
  void processNode(const aiNode *node, const aiScene *scene);
  Material processNodeMaterial(const aiMaterial *material);
  void createBoundingBox();

private:
  std::string mPath;
  std::string mName;
  glm::vec3 mPosition = glm::vec3(0.0f);
  glm::vec3 mScale = glm::vec3(1.0f);
  glm::vec3 mRotation = glm::vec3(0.0f);
  int mIndex;
  int mSceneIndex;
  std::vector<Mesh> mMeshes;
  glm::vec3 mMaxVert;
  glm::vec3 mMinVert;
};
