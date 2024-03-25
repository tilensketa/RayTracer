#pragma once

#include "Camera.h"
#include "CoordinateSystem.h"
#include "Scene.h"
#include "Settings.h"
#include "imgui.h"

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
  ChangeType overlayWindow();

  // Model Window
  bool loadModel();
  bool removeModel();
  void modelSelector();
  void deselectModel();
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
  void deselectLight();
  void refreshLoadedLights();

  // Debug Window
  bool viewportTypeEdit();
  void viewSelected();

  // Overlay Window
  bool drawCoordinateSystem(glm::vec3 &position, glm::vec3 &rotation,
                            glm::vec3 &scale);
  bool coordinateSystemModeEdit();
  bool handleScaleSystem(glm::vec3 &scale);
  bool handlePositionSystem(glm::vec3 &position);
  bool handleRotationSystem(glm::vec3 &rotation);
  ImVec2 worldToScreen(const glm::vec3 &worldPos);

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

  std::unique_ptr<CoordinateSystem> mCoordSystem;
  bool mDraggingX = false;
  bool mDraggingY = false;
  bool mDraggingZ = false;
  bool mRotatingX = false;
  bool mRotatingY = false;
  bool mRotatingZ = false;
};
