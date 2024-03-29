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
  // Windows
  void debugWindow(float fps, int dataSize);
  ChangeType cameraWindow();
  ChangeType overlayWindow();
  ChangeType selectorWindow();
  ChangeType propertiesWindow();
  ChangeType settingsWindow();

  // Refresh
  void refreshLoadedModels();
  void refreshLoadedLights();
  void refreshAvailableModels();
  void refreshDefaultModels();

  // Deselect
  void deselectAll();

  // Selector
  bool modelSelector();
  bool lightSelector();

  // Handle loading/removing model
  void handleRemovingModel(int index);
  bool removeModel();
  bool handleLoadingModel();
  void loadModel();

  // Handle loading/removing light
  void handleRemovingLight(int index);
  bool removeLight();
  bool handleLoadingLight();
  void loadLight();

  // Modify model/light
  ChangeType editModel();
  bool scaleModel();
  bool rotateModel();
  bool translateModel();
  bool materialEditor();
  bool albedoEdit();

  bool lightEdit();
  bool editPointLight();
  bool editDirectionalLight();

  // Debug
  bool viewportTypeEdit();
  void viewSelected();

  // Coordinate system
  bool drawCoordinateSystem(glm::vec3 &position, glm::vec3 &rotation,
                            glm::vec3 &scale);
  bool coordinateSystemModeEdit();
  bool handleScaleSystem(glm::vec3 &scale);
  bool handlePositionSystem(glm::vec3 &position);
  bool handleRotationSystem(glm::vec3 &rotation);
  ImVec2 worldToScreen(const glm::vec3 &worldPos);

public:
  std::shared_ptr<Scene> mScene;
  std::shared_ptr<Settings> mSettings;
  std::shared_ptr<Camera> mCamera;

private:
  // Available model
  std::string mModelsFolder;
  std::vector<std::string> mAvailableModelList;
  std::vector<std::string> mDefaultModelList;

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
