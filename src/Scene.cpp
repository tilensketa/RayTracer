#include "Scene.h"

int Scene::sModelsIndex = 0;
int Scene::sLightsIndex = 0;

void Scene::addModel(const std::string &modelName) {
  Model model(modelName);
  model.setIndex(sModelsIndex);
  std::string newName =
      model.getName() + "_" + std::to_string(model.getIndex());
  model.setName(newName);
  sModelsIndex++;
  mModels.push_back(model);
  recalculate();
}
void Scene::addLight(LightType type) {
  Light light(type);
  light.mIndex = sLightsIndex;
  light.setName();
  mLights.push_back(light);
  sLightsIndex++;
}
bool Scene::removeModel(const int modelIndex) {
  bool found = false;
  int index;
  for (int i = 0; i < mModels.size(); i++) {
    if (mModels[i].getIndex() == modelIndex) {
      index = i;
      found = true;
    }
  }
  if (!found)
    return false;

  auto modelIt = mModels.begin() + index;
  mModels.erase(modelIt);
  recalculate();
  return true;
}
bool Scene::removeLight(const int lightIndex) {
  bool found = false;
  int index;
  for (int i = 0; i < mLights.size(); i++) {
    if (mLights[i].mIndex == lightIndex) {
      index = i;
      found = true;
    }
  }
  if (!found)
    return false;

  auto ligthIt = mLights.begin() + index;
  mLights.erase(ligthIt);
  return true;
}

Model *Scene::getModel(int modelIndex) {
  for (Model &model : mModels) {
    if (model.getIndex() == modelIndex)
      return &model;
  }
  std::cout << "model not found id: " << modelIndex << std::endl;
  return nullptr;
}

Light *Scene::getLight(int lightIndex) {
  for (Light &light : mLights) {
    if (light.mIndex == lightIndex)
      return &light;
  }
  std::cout << "light not found id: " << lightIndex << std::endl;
  return nullptr;
}

void Scene::recalculate() {
  int modelIndex = 0;
  mTriangles.clear();
  mVertices.clear();
  mMaterials.clear();
  mMaterialIndexes.clear();
  int indicesOffset = 0;
  for (Model &model : mModels) {
    model.setSceneIndex(modelIndex);
    modelIndex++;
    mMaterialIndexes.push_back(model.getMeshCount());
    for (Mesh &mesh : model.modMeshes()) {
      const Material material = mesh.getMaterial();
      mMaterials.push_back(material);
      for (Triangle &triangle : mesh.modTriangles()) {
        triangle.mModedIndices[0] = triangle.mIndices[0] + indicesOffset;
        triangle.mModedIndices[1] = triangle.mIndices[1] + indicesOffset;
        triangle.mModedIndices[2] = triangle.mIndices[2] + indicesOffset;
        mTriangles.push_back(triangle);
      }
      for (const Vertex &vertex : mesh.getVertices()) {
        mVertices.push_back(vertex);
      }
      indicesOffset += mesh.getVerticesCount();
    }
  }
}
