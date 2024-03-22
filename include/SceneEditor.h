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
  bool cameraWindow();
  bool lightsWindow();

  // Model
  bool loadModel();
  bool removeModel();
  void modelSelector();
  void refreshLoadedModels();
  void refreshAvailableModels();

  bool transformModel();
  bool scaleModel();
  bool rotateModel();
  bool translateModel();

  bool materialEditor();
  bool albedoEdit();

  // Lights
  bool loadLight();
  bool removeLight();
  void lightSelector();
  bool lightEdit();
  void refreshLoadedLights();

  bool editVec3WithColorEdit3(const char *label, int index, glm::vec3 &vec);
  bool editVec3(const char *text, glm::vec3 &vec, const char *opt[3], float min,
                float max);
  bool editSliderFloat(const char *text, float &value, float min, float max);

public:
  std::shared_ptr<Scene> mScene;
  std::shared_ptr<Settings> mSettings;
  std::shared_ptr<Camera> mCamera;

private:
  // Available model
  std::string mModelsFolder;
  std::vector<std::string> mAvailableModelList;

  // Select model
  int mSelectedModelIndex = -1;
  Model *mSelectedModel = nullptr;
  std::vector<const char *> mLoadedModelList;

  // Select light
  int mSelectedLightIndex = -1;
  Light *mSelectedLight = nullptr;
  std::vector<const char *> mLoadedLightsList;
};
