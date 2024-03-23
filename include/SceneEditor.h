#pragma once

#include "Camera.h"
#include "Scene.h"
#include "Settings.h"

#include <memory>
#include <string>

enum ChangeType {
  NoneType = 0,
  CameraType,
  SettingsType,
  MaterialType,
  LightType,
  BVHType
};

class SceneEditor {
public:
  SceneEditor(const std::string &modelsFolder, std::shared_ptr<Scene> scene,
              std::shared_ptr<Camera> camera,
              std::shared_ptr<Settings> settings);

  ChangeType render(float fps, int dataSize);

private:
  ChangeType debugWindow(float fps, int dataSize);
  ChangeType modelWindow();
  ChangeType cameraWindow();
  ChangeType lightsWindow();

  // Model Window
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

  // Lights Window
  bool loadLight();
  bool removeLight();
  bool lightEdit();
  void lightSelector();
  void refreshLoadedLights();

  // Debug Window
  bool viewportTypeEdit();
  void viewSelected();

  bool editVec3WithColorEdit3(const char *label, int index, glm::vec3 &vec);
  bool editVec3(const char *text, glm::vec3 &vec, const char *opt[3], float min,
                float max);
  bool editSliderFloat(const char *text, float &value, float min, float max);
  bool editSliderInt(const char *text, int &value, int min, int max);

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
