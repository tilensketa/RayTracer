#pragma once

#include "Light.h"
#include "Model.h"
#include <iostream>
#include <memory>
#include <vector>

class Scene {
public:
  Scene() = default;

  void addModel(const std::string &modelName);
  void addLight(LightType type);
  bool removeModel(const int modelIndex);
  bool removeLight(const int lightIndex);
  void recalculate();

  // Model
  const int getModelCount() const { return mModels.size(); }
  const std::vector<Model> &getModels() const { return mModels; }
  Model *getModel(int index);
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

  // Lights
  const std::vector<Light> &getLights() const { return mLights; }
  const int getLightsCount() const { return mLights.size(); }
  Light *getLight(int index);

private:
  static int sModelsIndex;
  static int sLightsIndex;

private:
  std::vector<Model> mModels;
  std::vector<Triangle> mTriangles;
  std::vector<Vertex> mVertices;
  std::vector<Material> mMaterials;
  std::vector<int> mMaterialIndexes;
  std::vector<Light> mLights;
};
