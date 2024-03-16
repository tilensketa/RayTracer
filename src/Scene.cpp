#include "Scene.h"

void Scene::add(Model model) {
  model.setIndex(mModels.size());
  mModels.push_back(model);
  recalculate();
}
void Scene::remove(std::string modelNameToRemove) {
  int index = 0;
  bool modelFound = false;
  for (int i = 0; i < mModels.size(); i++) {
    if (mModels[i].getName() == modelNameToRemove) {
      index = i;
      modelFound = true;
    }
  }
  if (!modelFound)
    return;

  auto modelIt = mModels.begin() + index;
  mModels.erase(modelIt);
  recalculate();
}

std::unique_ptr<Model> Scene::getModel(int index) {
  if (index >= 0 && index < mModels.size()) {
    return std::make_unique<Model>(mModels[index]);
  } else {
    return nullptr;
  }
}

void Scene::recalculate() {
  int index = 0;
  mTriangles.clear();
  mVertices.clear();
  mMaterials.clear();
  mMaterialIndexes.clear();
  int indicesOffset = 0;
  for (Model &model : mModels) {
    model.setIndex(index);
    index++;
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
