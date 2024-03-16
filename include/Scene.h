#pragma once

#include "Model.h"
#include <iostream>
#include <memory>
#include <vector>

class Scene {
public:
  Scene() = default;

  void add(Model model);
  void remove(std::string modelNameToRemove);

  // Model
  const int getModelCount() const { return mModels.size(); }
  const std::vector<Model> &getModels() const { return mModels; }
  std::unique_ptr<Model> getModel(int i);
  std::vector<Model> &modModels() { return mModels; }

  // Triangles
  const std::vector<Triangle> &getTriangles() const { return mTriangles; }
  const int getTrianglesCount() const { return mTriangles.size(); }

  // Vertices
  const std::vector<Vertex> &getVertices() const { return mVertices; }
  const int getVerticesCount() const { return mVertices.size(); }

  // Material
  const std::vector<Material> &getMaterials() const { return mMaterials; }
  const int getMaterialsCount() const { return mMaterialIndexes.size(); }
  const std::vector<int> &getMaterialIndexes() const {
    return mMaterialIndexes;
  }

private:
  void recalculate();

private:
  std::vector<Model> mModels;
  std::vector<Triangle> mTriangles;
  std::vector<Vertex> mVertices;
  std::vector<Material> mMaterials;
  std::vector<int> mMaterialIndexes;
};
