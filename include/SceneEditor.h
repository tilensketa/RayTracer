#pragma once

#include "Scene.h"
#include "Settings.h"

#include <memory>
#include <string>

class SceneEditor {
public:
  SceneEditor(const std::string &modelsFolder, std::shared_ptr<Scene> scene,
              std::shared_ptr<Settings> settings);

  bool render(float fps, int dataSize);

private:
  bool debugWindow(float fps, int dataSize);
  bool modelWindow();
  bool loadModel();
  bool removeModel();
  bool translateModel();
  void modelSelector();
  bool materialEditor();

  void refreshLoadedModels();
  bool EditVec3WithColorEdit3(const char* label, glm::vec3& vec);

public:
  std::shared_ptr<Scene> mScene;
  std::shared_ptr<Settings> mSettings;

private:
  // Load model
  std::string mModelsFolder;
  std::vector<std::string> mModelList;
  int mSelectedLoadModelIndex = 0;
  bool mShowLoadModelPopup = false;

  // Select model
  std::unique_ptr<Model> mSelectedModel = nullptr;
  std::vector<const char *> mLoadedModelList;
};
