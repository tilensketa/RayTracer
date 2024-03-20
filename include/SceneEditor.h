#pragma once

#include "Camera.h"
#include "Scene.h"
#include "Settings.h"

#include <memory>
#include <string>

class SceneEditor {
public:
  SceneEditor(const std::string &modelsFolder, std::shared_ptr<Scene> scene,
              std::shared_ptr<Camera> camera,
              std::shared_ptr<Settings> settings);

  bool render(float fps, int dataSize);

private:
  bool debugWindow(float fps, int dataSize);
  bool modelWindow();

  bool loadModel();
  bool removeModel();
  void modelSelector();

  bool transformModel();
  bool scaleModel();
  bool rotateModel();
  bool translateModel();

  bool materialEditor();
  bool albedoEdit();

  bool editCamera();

  void refreshLoadedModels();
  bool editVec3WithColorEdit3(const char *label, int index, glm::vec3 &vec);
  bool editVec3(const char *text, glm::vec3 &vec, const char *opt[3],
                float rangeMin, float rangeMax);
  bool editSliderFloat(const char *label, float &value, float min, float max,
                       int index);

public:
  std::shared_ptr<Scene> mScene;
  std::shared_ptr<Settings> mSettings;
  std::shared_ptr<Camera> mCamera;

private:
  // Load model
  std::string mModelsFolder;
  std::vector<std::string> mModelList;
  int mSelectedLoadModelIndex = 0;
  bool mShowLoadModelPopup = false;

  // Select model
  int mSelectedModelIndex = -1;
  Model *mSelectedModel = nullptr;
  std::vector<const char *> mLoadedModelList;

  // Transform model
  bool mShowTransformHeader = true;
};
