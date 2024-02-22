#pragma once

#include "Model.h"
#include <vector>

class Scene {
public:
  Scene() = default;

  void add(Model &model) {
    model.setIndex(mModels.size());
    mModels.push_back(model);
  }

  const int getModelCount() const { return mModels.size(); }
  const std::vector<Model> &getModels() const { return mModels; }

  std::vector<Model> &modModels() { return mModels; }

private:
  std::vector<Model> mModels;
};
