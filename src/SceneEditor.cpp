#include "SceneEditor.h"
#include "Edit.h"

#include "glm/gtc/matrix_transform.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <filesystem>
#include <string>
namespace fs = std::filesystem;

#define RED_IMGUI IM_COL32(255, 0, 0, 255)
#define GREEN_IMGUI IM_COL32(0, 255, 0, 255)
#define BLUE_IMGUI IM_COL32(0, 0, 255, 255)
#define WHITE_IMGUI IM_COL32(255, 255, 255, 255)

SceneEditor::SceneEditor(const std::string &modelsFolder,
                         std::shared_ptr<Scene> scene,
                         std::shared_ptr<Camera> camera,
                         std::shared_ptr<Settings> settings)
    : mModelsFolder(modelsFolder), mScene(scene), mCamera(camera),
      mSettings(settings) {

  refreshAvailableModels();
  refreshDefaultModels();
  refreshLoadedModels();
  refreshLoadedLights();

  mSelectedModelIndex = 0;
  mSelectedModel = mScene->getModel(mSelectedModelIndex);

  mCoordSystem = std::make_unique<CoordinateSystem>();
}

ChangeType SceneEditor::render(float fps, int dataSize) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ChangeType settingsChange = settingsWindow();
  ChangeType cameraChange = cameraWindow();
  ChangeType selectorChange = selectorWindow();
  ChangeType overlayChange = overlayWindow();
  ChangeType propertiesChange = propertiesWindow();
  debugWindow(fps, dataSize);

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  if (!settingsChange && !selectorChange && !cameraChange && !overlayChange &&
      !propertiesChange)
    return ChangeType::NoneType;
  if (settingsChange == ChangeType::BVHType ||
      selectorChange == ChangeType::BVHType ||
      overlayChange == ChangeType::BVHType ||
      propertiesChange == ChangeType::BVHType)
    return ChangeType::BVHType;
  if (propertiesChange == ChangeType::MaterialType)
    return ChangeType::MaterialType;
  if (settingsChange == ChangeType::SettingsType)
    return ChangeType::SettingsType;
  if (selectorChange == ChangeType::LightType ||
      overlayChange == ChangeType::LightType ||
      propertiesChange == ChangeType::LightType)
    return ChangeType::LightType;
  if (cameraChange == ChangeType::CameraType)
    return ChangeType::CameraType;
  return ChangeType::NoneType;
}

ChangeType SceneEditor::selectorWindow() {
  ImGui::Begin("Scene");
  ImGui::Text("Models");
  bool modelChange = modelSelector();
  ImGui::Text("Lights");
  bool lightChange = lightSelector();
  ImGui::End();

  if (modelChange)
    return ChangeType::BVHType;
  else if (lightChange)
    return ChangeType::LightType;
  return ChangeType::NoneType;
}

void SceneEditor::debugWindow(float fps, int dataSize) {
  ImGui::Begin("DEBUG");
  ImGui::Text("FPS: %f", fps);
  ImGui::Text("Models: %i", mScene->getModelCount());
  ImGui::Text("Triangles: %i", mScene->getTrianglesCount());
  ImGui::Text("Vertices: %i", mScene->getVerticesCount());
  ImGui::Text("Materials: %i", mScene->getMaterialsCount());
  ImGui::Text("Data: %i", dataSize);
  viewSelected();
  ImGui::End();
}

ChangeType SceneEditor::settingsWindow() {
  ImGui::Begin("SETTINGS");
  bool depthChange = Edit::slider("BVH depth", mSettings->mMaxDepth, 0, 30);
  bool triangleChange =
      Edit::slider("BVH triangles", mSettings->mMaxTrianglesInLeaf, 0, 100);
  bool viewportModeChange = viewportTypeEdit();
  bool coordinateModeChange = coordinateSystemModeEdit();
  bool downsampleChange =
      Edit::slider("Downsample", mSettings->mDownsampleFactor, 1, 20);

  ImGui::End();
  if (depthChange || triangleChange)
    return ChangeType::BVHType;
  if (downsampleChange || viewportModeChange)
    return ChangeType::SettingsType;
  return ChangeType::NoneType;
}

bool SceneEditor::viewportTypeEdit() {
  bool viewportModeChange = false;
  if (ImGui::RadioButton("Flat", (int *)&mSettings->mViewportMode, Flat)) {
    viewportModeChange = true;
  }
  ImGui::SameLine();
  if (ImGui::RadioButton("Shaded", (int *)&mSettings->mViewportMode, Shaded)) {
    viewportModeChange = true;
  }
  ImGui::SameLine();
  if (ImGui::RadioButton("Wireframe", (int *)&mSettings->mViewportMode,
                         Wireframe)) {
    viewportModeChange = true;
  }
  return viewportModeChange;
}

void SceneEditor::viewSelected() {
  ImGui::Text("Selected model:");
  if (mSelectedModel != nullptr)
    ImGui::Text("%s : %i", mSelectedModel->getPath().c_str(),
                mSelectedModelIndex);
  else
    ImGui::Text("None : %i", mSelectedModelIndex);

  ImGui::Text("Selected light:");
  if (mSelectedLight != nullptr)
    ImGui::Text("%s : %i", mSelectedLight->mName.c_str(), mSelectedLightIndex);
  else
    ImGui::Text("None : %i", mSelectedLightIndex);
}

bool SceneEditor::scaleModel() {
  bool scaleChange = Edit::vec3("Scale", mSelectedModel->modScale());
  if (scaleChange) {
    mSelectedModel->update();
    mScene->recalculate();
  }
  return scaleChange;
}

bool SceneEditor::rotateModel() {
  bool rotateChange = Edit::vec3("Rotate", mSelectedModel->modRotation());
  if (rotateChange) {
    mSelectedModel->update();
    mScene->recalculate();
  }
  return rotateChange;
}

bool SceneEditor::translateModel() {
  bool translateChange = Edit::vec3("Position", mSelectedModel->modPosition());
  if (translateChange) {
    mSelectedModel->update();
    mScene->recalculate();
  }
  return translateChange;
}

bool SceneEditor::modelSelector() {
  bool modelRemoved = false;
  bool modelLoaded = false;

  ImVec2 size(-1, 10 * ImGui::GetTextLineHeightWithSpacing());

  if (ImGui::BeginListBox("LoadedModels", size)) {
    for (const Model &model : mScene->getModels()) {
      std::string name = model.getName();
      if (ImGui::Selectable(name.c_str(),
                            mSelectedModelIndex == model.getIndex())) {
        deselectAll();
        mSelectedModelIndex = model.getIndex();
        /* mSelectedModel = mScene->getModel(
            mScene->getModels()[mSelectedModelIndex].getIndex()); */
        mSelectedModel = mScene->getModel(mSelectedModelIndex);
      }
      handleRemovingModel(model.getSceneIndex());
    }
    if (removeModel())
      modelRemoved = true;

    if (handleLoadingModel())
      modelLoaded = true;

    ImGui::EndListBox();
  }
  if (modelLoaded) {
    loadModel();
  }
  return modelRemoved || modelLoaded;
}

void SceneEditor::handleRemovingModel(int index) {
  if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
    mSelectedModelIndex = index;
    mSelectedModel =
        mScene->getModel(mScene->getModels()[mSelectedModelIndex].getIndex());
    if (!ImGui::IsPopupOpen("RemoveModelMenu")) {
      ImGui::OpenPopup("RemoveModelMenu");
    }
  }
}

bool SceneEditor::removeModel() {
  bool modelRemoved = false;
  if (ImGui::BeginPopup("RemoveModelMenu")) {
    if (ImGui::MenuItem("RemoveModel")) {
      mScene->removeModel(mSelectedModel->getIndex());
      modelRemoved = true;
      deselectAll();
      refreshLoadedModels();
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
  return modelRemoved;
}

bool SceneEditor::handleLoadingModel() {
  bool modelLoaded = false;
  if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)) {
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
      ImGui::OpenPopup("AddModelMenu");
    }
  }
  if (ImGui::BeginPopup("AddModelMenu")) {
    if (ImGui::BeginMenu("Default")) {
      for (const std::string defaultModelName : mDefaultModelList) {
        if (ImGui::MenuItem(defaultModelName.c_str())) {
          mScene->addModel(mModelsFolder + "Default/" + defaultModelName);
          modelLoaded = true;
        }
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Custom")) {
      for (const std::string availableModelName : mAvailableModelList) {
        if (ImGui::MenuItem(availableModelName.c_str())) {
          mScene->addModel(mModelsFolder + availableModelName);
          modelLoaded = true;
        }
      }
      ImGui::EndMenu();
    }
    ImGui::EndPopup();
  }
  return modelLoaded;
}

void SceneEditor::loadModel() {
  deselectAll();
  mSelectedModelIndex = mScene->getModelCount() - 1;
  mSelectedModel =
      mScene->getModel(mScene->getModels()[mSelectedModelIndex].getIndex());
  refreshLoadedModels();
}

bool SceneEditor::materialEditor() {
  bool isChanged = false;
  ImGui::Text("MATERIAL");
  if (albedoEdit())
    isChanged = true;
  return isChanged;
}

bool SceneEditor::albedoEdit() {
  bool isChanged = false;
  int index = 0;
  for (Mesh &mesh : mSelectedModel->modMeshes()) {
    if (Edit::colorEdit3("Diffuse", mesh.modMaterial().modDiffuse())) {
      isChanged = true;
    }
    index++;
  }
  if (isChanged) {
    mScene->recalculate();
  }
  return isChanged;
}

ChangeType SceneEditor::cameraWindow() {
  ImGui::Begin("Camera");

  bool fovChanged = Edit::slider("FOV", mCamera->modFOV(), 30.0f, 90.0f);
  bool moveChanged =
      Edit::slider("Move Speed", mCamera->modMoveSpeed(), 0.0f, 10.0f);
  bool rotateChanged =
      Edit::slider("Rotate Speed", mCamera->modRotateSpeed(), 0.0f, 2.0f);

  ImGui::End();
  if (fovChanged || moveChanged || rotateChanged)
    return ChangeType::CameraType;
  return ChangeType::NoneType;
}

void SceneEditor::refreshLoadedModels() {
  mLoadedModelList.clear();
  for (const Model &model : mScene->getModels()) {
    mLoadedModelList.push_back(model.getName().c_str());
  }
}

void SceneEditor::refreshLoadedLights() {
  mLoadedLightsList.clear();
  for (const Light &light : mScene->getLights()) {
    mLoadedLightsList.push_back(light.mName.c_str());
  }
}
void SceneEditor::refreshAvailableModels() {
  mAvailableModelList.clear();
  for (const auto &entry : fs::directory_iterator(mModelsFolder)) {
    std::string extension = entry.path().extension().string();
    if (extension == ".obj" || extension == ".fbx") {
      std::string filename = entry.path().filename().string();
      mAvailableModelList.push_back(filename);
    }
  }
}

void SceneEditor::refreshDefaultModels() {
  mDefaultModelList.clear();
  for (const auto &entry : fs::directory_iterator(mModelsFolder + "Default/")) {
    std::string extension = entry.path().extension().string();
    if (extension == ".obj" || extension == ".fbx") {
      std::string filename = entry.path().filename().string();
      mDefaultModelList.push_back(filename);
    }
  }
}

bool SceneEditor::lightSelector() {
  bool lightRemoved = false;
  bool lightLoaded = false;

  ImVec2 size(-1, 10 * ImGui::GetTextLineHeightWithSpacing());

  if (ImGui::BeginListBox("LoadedLights", size)) {
    for (const Light &light : mScene->getLights()) {
      std::string name = light.mName;
      if (ImGui::Selectable(name.c_str(),
                            mSelectedLightIndex == light.mIndex)) {
        deselectAll();
        mSelectedLightIndex = light.mIndex;
        mSelectedLight = mScene->getLight(mSelectedLightIndex);
      }
      handleRemovingLight(light.mIndex);
    }
    if (removeLight())
      lightRemoved = true;

    if (handleLoadingLight())
      lightLoaded = true;

    ImGui::EndListBox();
  }
  if (lightLoaded) {
    loadLight();
  }
  return lightLoaded || lightRemoved;
}

void SceneEditor::handleRemovingLight(int index) {
  if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
    mSelectedLightIndex = index;
    mSelectedLight = mScene->getLight(mSelectedLightIndex);
    if (!ImGui::IsPopupOpen("RemoveLightMenu")) {
      ImGui::OpenPopup("RemoveLightMenu");
    }
  }
}

bool SceneEditor::removeLight() {
  bool lightRemoved = false;
  if (ImGui::BeginPopup("RemoveLightMenu")) {
    if (ImGui::MenuItem("RemoveLight")) {
      mScene->removeLight(mSelectedLight->mIndex);
      lightRemoved = true;
      mSelectedLight = nullptr;
      mSelectedLightIndex = -1;
      refreshLoadedLights();
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
  return lightRemoved;
}

bool SceneEditor::handleLoadingLight() {
  bool lightLoaded = false;
  if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)) {
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
      ImGui::OpenPopup("AddLightMenu");
    }
  }
  if (ImGui::BeginPopup("AddLightMenu")) {
    if (ImGui::MenuItem("Point Light")) {
      mScene->addLight(LightType::Point);
      lightLoaded = true;
    }
    if (ImGui::MenuItem("Directional Light")) {
      mScene->addLight(LightType::Directional);
      lightLoaded = true;
    }
    ImGui::EndPopup();
  }
  return lightLoaded;
}

void SceneEditor::loadLight() {
  deselectAll();
  mSelectedLightIndex = mScene->getLightsCount() - 1;
  mSelectedLight =
      mScene->getLight(mScene->getLights()[mSelectedLightIndex].mIndex);
  refreshLoadedLights();
}

void SceneEditor::deselectAll() {
  mSelectedModel = nullptr;
  mSelectedModelIndex = -1;
  mSelectedLight = nullptr;
  mSelectedLightIndex = -1;
}

bool SceneEditor::lightEdit() {
  if (mSelectedLight == nullptr)
    return false;
  bool lightChanged = false;
  if (mSelectedLight->mType == LightType::Point) {
    if (editPointLight())
      lightChanged = true;
  } else if (mSelectedLight->mType == LightType::Directional) {
    if (editDirectionalLight())
      lightChanged = true;
  }
  return lightChanged;
}

bool SceneEditor::editPointLight() {
  bool positionChanged = Edit::vec3("Position", mSelectedLight->mPosition);
  bool constantChanged =
      Edit::slider("Intensity", mSelectedLight->mIntensity, 0, 5);
  bool colorChanged = Edit::colorEdit3("Color", mSelectedLight->mColor);
  return positionChanged || constantChanged || colorChanged;
}

bool SceneEditor::editDirectionalLight() {
  bool positionChanged = Edit::vec3("Position", mSelectedLight->mPosition);
  bool intensityChanged =
      Edit::slider("Intensity", mSelectedLight->mIntensity, 0, 5);
  bool pitchChanged = Edit::slider("Pitch", mSelectedLight->mPitch, 0, 360);
  bool yawChanged = Edit::slider("Yaw", mSelectedLight->mYaw, 0, 360);
  bool colorChanged = Edit::colorEdit3("Color", mSelectedLight->mColor);
  return positionChanged || intensityChanged || pitchChanged || yawChanged ||
         colorChanged;
}

ChangeType SceneEditor::propertiesWindow() {
  ChangeType modelChange = ChangeType::NoneType;
  bool lightChange = false;

  ImGui::Begin("Properties");
  if (mSelectedModel != nullptr) {
    ImGui::Text("%s", mSelectedModel->getName().c_str());
    ImGui::Separator();
    modelChange = editModel();
  } else if (mSelectedLight != nullptr) {
    ImGui::Text("%s", mSelectedLight->mName.c_str());
    ImGui::Separator();
    if (lightEdit())
      lightChange = true;
  }
  ImGui::End();

  if (modelChange == ChangeType::BVHType)
    return ChangeType::BVHType;
  else if (modelChange == ChangeType::MaterialType)
    return ChangeType::MaterialType;
  else if (lightChange)
    return ChangeType::LightType;
  return ChangeType::NoneType;
}

ChangeType SceneEditor::editModel() {
  ImGui::Text("TRANSFORM");
  bool translateChange = translateModel();
  bool scaleChange = scaleModel();
  bool rotateChange = rotateModel();
  bool materialChange = materialEditor();

  if (translateChange || scaleChange || rotateChange)
    return ChangeType::BVHType;
  else if (materialChange)
    return ChangeType::MaterialType;
  return ChangeType::NoneType;
}

ChangeType SceneEditor::overlayWindow() {
  bool modelChanged = false;
  bool lightChanged = false;
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
  ImGui::Begin("Transparent Window", NULL,
               ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
                   ImGuiWindowFlags_NoInputs);

  if (mSelectedModel != nullptr) {
    if (drawCoordinateSystem(mSelectedModel->modPosition(),
                             mSelectedModel->modRotation(),
                             mSelectedModel->modScale())) {
      modelChanged = true;
      mSelectedModel->update();
      mScene->recalculate();
    }
  }
  if (mSelectedLight != nullptr) {
    mCoordSystem->mMode = Mode::Position;
    if (drawCoordinateSystem(mSelectedLight->mPosition, mSelectedLight->mColor,
                             mSelectedLight->mColor)) {
      lightChanged = true;
    }
  }

  ImGui::End();
  if (modelChanged)
    return ChangeType::BVHType;
  if (lightChanged)
    return ChangeType::LightType;
  return ChangeType::NoneType;
}

float Distance(const ImVec2 &p1, const ImVec2 &p2) {
  return std::sqrt((p1.x - p2.x) * (p1.x - p2.x) +
                   (p1.y - p2.y) * (p1.y - p2.y));
}

bool SceneEditor::coordinateSystemModeEdit() {
  bool modeChange = false;
  if (ImGui::RadioButton("Pos", (int *)&mCoordSystem->mMode, Position)) {
    modeChange = true;
  }
  ImGui::SameLine();
  if (ImGui::RadioButton("Rot", (int *)&mCoordSystem->mMode, Rotation)) {
    modeChange = true;
  }
  ImGui::SameLine();
  if (ImGui::RadioButton("Scale", (int *)&mCoordSystem->mMode, Scale)) {
    modeChange = true;
  }
  return modeChange;
}

bool SceneEditor::drawCoordinateSystem(glm::vec3 &position, glm::vec3 &rotation,
                                       glm::vec3 &scale) {
  bool isChanged = false;

  mCoordSystem->mPosition = position;
  mCoordSystem->mRotation = rotation;
  mCoordSystem->update();

  if (mCoordSystem->mMode == Mode::Position) {
    if (handlePositionSystem(position))
      isChanged = true;
  } else if (mCoordSystem->mMode == Mode::Rotation) {
    if (handleRotationSystem(rotation))
      isChanged = true;
  } else if (mCoordSystem->mMode == Mode::Scale) {
    if (handleScaleSystem(scale))
      isChanged = true;
  }

  return isChanged;
}

bool SceneEditor::handleScaleSystem(glm::vec3 &scale) {
  bool isChanged = false;
  float thick = 2.0f;
  float radius = 10.0f;

  ImVec2 origin = worldToScreen(mCoordSystem->mCenter.mModedPosition);
  ImVec2 scaleAxis[3];
  for (int i = 0; i < 3; i++) {
    scaleAxis[i] = worldToScreen(mCoordSystem->mScaleAxis[i].mModedPosition);
  }

  static ImVec2 startX, startY, startZ;

  ImVec2 mousePos = ImGui::GetIO().MousePos;
  // Check if mouse is pressed within the coordinate system's X-axis
  if (!mDraggingX && !mDraggingY && !mDraggingZ &&
      ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    if (Distance(mousePos, scaleAxis[0]) <= radius) {
      mDraggingX = true;
      startX = mousePos;
    } else if (Distance(mousePos, scaleAxis[1]) <= radius) {
      mDraggingY = true;
      startY = mousePos;
    } else if (Distance(mousePos, scaleAxis[2]) <= radius) {
      mDraggingZ = true;
      startZ = mousePos;
    }
  }
  // If dragging in X-axis, update position of the coordinate system
  if (mDraggingX) {
    float deltaPixels = mousePos.x - startX.x;
    float deltaX = deltaPixels *
                   (mCoordSystem->mPosition.x -
                    mCoordSystem->mPositionAxis[0].mModedPosition.x) /
                   (scaleAxis[0].x - origin.x);
    scale.x -= deltaX;
    startX = mousePos;
    isChanged = true;
  }
  // If dragging in Y-axis, update position of the coordinate system
  else if (mDraggingY) {
    float deltaPixels = mousePos.y - startY.y;
    float deltaY = deltaPixels *
                   (mCoordSystem->mPosition.y -
                    mCoordSystem->mPositionAxis[1].mModedPosition.y) /
                   (scaleAxis[1].y - origin.y);
    scale.y -= deltaY;
    startY = mousePos;
    isChanged = true;
  }
  // If dragging in Z-axis, update position of the coordinate system
  else if (mDraggingZ) {
    float deltaPixels = mousePos.x - startZ.x;
    float deltaZ = deltaPixels *
                   (mCoordSystem->mPosition.z -
                    mCoordSystem->mPositionAxis[2].mModedPosition.z) /
                   (scaleAxis[2].x - origin.x);
    scale.z -= deltaZ;
    startZ = mousePos;
    isChanged = true;
  }

  if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
    mDraggingX = false;
    mDraggingY = false;
    mDraggingZ = false;
  }

  ImGui::GetWindowDrawList()->AddLine(origin, scaleAxis[0], RED_IMGUI, thick);
  ImGui::GetWindowDrawList()->AddLine(origin, scaleAxis[1], GREEN_IMGUI, thick);
  ImGui::GetWindowDrawList()->AddLine(origin, scaleAxis[2], BLUE_IMGUI, thick);
  ImGui::GetWindowDrawList()->AddCircleFilled(origin, radius, WHITE_IMGUI);
  ImGui::GetWindowDrawList()->AddCircleFilled(scaleAxis[0], radius, RED_IMGUI);
  ImGui::GetWindowDrawList()->AddCircleFilled(scaleAxis[1], radius,
                                              GREEN_IMGUI);
  ImGui::GetWindowDrawList()->AddCircleFilled(scaleAxis[2], radius, BLUE_IMGUI);

  return isChanged;
}

bool SceneEditor::handlePositionSystem(glm::vec3 &position) {
  bool isChanged = false;
  float thick = 2.0f;
  float radius = 10.0f;

  ImVec2 origin = worldToScreen(mCoordSystem->mCenter.mModedPosition);
  ImVec2 positionAxis[3];
  for (int i = 0; i < 3; i++) {
    positionAxis[i] =
        worldToScreen(mCoordSystem->mPositionAxis[i].mModedPosition);
  }

  ImVec2 xyPlane[5];
  ImVec2 xzPlane[5];
  ImVec2 yzPlane[5];
  for (int i = 0; i < 5; i++) {
    xyPlane[i] = worldToScreen(mCoordSystem->mPlaneXY[i].mModedPosition);
    xzPlane[i] = worldToScreen(mCoordSystem->mPlaneXZ[i].mModedPosition);
    yzPlane[i] = worldToScreen(mCoordSystem->mPlaneYZ[i].mModedPosition);
  }

  static ImVec2 startX, startY, startZ;
  static ImVec2 startXY, startXZ, startYZ;

  ImVec2 mousePos = ImGui::GetIO().MousePos;

  // Check if mouse is pressed within the coordinate system's X-axis
  if (!mDraggingX && !mDraggingY && !mDraggingZ &&
      ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
    if (Distance(mousePos, xyPlane[4]) <= radius) {
      mDraggingX = true;
      mDraggingY = true;
      startXY = mousePos;
    } else if (Distance(mousePos, xzPlane[4]) <= radius) {
      mDraggingX = true;
      mDraggingZ = true;
      startXZ = mousePos;
    } else if (Distance(mousePos, yzPlane[4]) <= radius) {
      mDraggingY = true;
      mDraggingZ = true;
      startYZ = mousePos;
    } else if (Distance(mousePos, positionAxis[0]) <= radius) {
      mDraggingX = true;
      startX = mousePos;
    } else if (Distance(mousePos, positionAxis[1]) <= radius) {
      mDraggingY = true;
      startY = mousePos;
    } else if (Distance(mousePos, positionAxis[2]) <= radius) {
      mDraggingZ = true;
      startZ = mousePos;
    }
  }

  if (mDraggingX && mDraggingY) {
    // Dragging in both X and Y axes
    ImVec2 delta = ImVec2(mousePos.x - startXY.x, mousePos.y - startXY.y);
    float deltaX =
        delta.x *
        (position.x - mCoordSystem->mPositionAxis[0].mModedPosition.x) /
        (positionAxis[0].x - origin.x);
    float deltaY =
        delta.y *
        (position.y - mCoordSystem->mPositionAxis[1].mModedPosition.y) /
        (positionAxis[1].y - origin.y);
    position.x -= deltaX;
    position.y -= deltaY;
    isChanged = true;
  } else if (mDraggingX && mDraggingZ) {
    // Dragging in XZ axes
    ImVec2 delta = ImVec2(mousePos.x - startXZ.x, mousePos.y - startXZ.y);
    float deltaX =
        delta.x *
        (position.x - mCoordSystem->mPositionAxis[0].mModedPosition.x) /
        (positionAxis[0].x - origin.x);
    float deltaZ =
        delta.y *
        (position.z - mCoordSystem->mPositionAxis[2].mModedPosition.z) /
        (positionAxis[2].y - origin.y);
    position.x -= deltaX;
    position.z -= deltaZ;
    isChanged = true;
  } else if (mDraggingY && mDraggingZ) {
    // Dragging in YZ axes
    ImVec2 delta = ImVec2(mousePos.x - startYZ.x, mousePos.y - startYZ.y);
    float deltaY =
        delta.y *
        (position.y - mCoordSystem->mPositionAxis[1].mModedPosition.y) /
        (positionAxis[1].y - origin.y);
    float deltaZ =
        delta.x *
        (position.z - mCoordSystem->mPositionAxis[2].mModedPosition.z) /
        (positionAxis[2].x - origin.x);
    position.y -= deltaY;
    position.z -= deltaZ;
    isChanged = true;
  }

  // If dragging in X-axis, update position of the coordinate system
  else if (mDraggingX) {
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    float deltaPixels = mousePos.x - startX.x;
    float deltaX =
        deltaPixels *
        (position.x - mCoordSystem->mPositionAxis[0].mModedPosition.x) /
        (positionAxis[0].x - origin.x);
    position.x -= deltaX;
    isChanged = true;
  }
  // If dragging in Y-axis, update position of the coordinate system
  else if (mDraggingY) {
    float deltaPixels = mousePos.y - startY.y;
    float deltaY =
        deltaPixels *
        (position.y - mCoordSystem->mPositionAxis[1].mModedPosition.y) /
        (positionAxis[1].y - origin.y);
    position.y -= deltaY;
    isChanged = true;
  }
  // If dragging in Z-axis, update position of the coordinate system
  else if (mDraggingZ) {
    float deltaPixels = mousePos.x - startZ.x;
    float deltaZ =
        deltaPixels *
        (position.z - mCoordSystem->mPositionAxis[2].mModedPosition.z) /
        (positionAxis[2].x - origin.x);
    position.z -= deltaZ;
    isChanged = true;
  }
  if (isChanged) {
    startX = mousePos;
    startY = mousePos;
    startZ = mousePos;
    startXY = mousePos;
    startXZ = mousePos;
    startYZ = mousePos;
  }

  if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
    mDraggingX = false;
    mDraggingY = false;
    mDraggingZ = false;
  }

  ImGui::GetWindowDrawList()->AddLine(origin, positionAxis[0], RED_IMGUI,
                                      thick);
  ImGui::GetWindowDrawList()->AddLine(origin, positionAxis[1], GREEN_IMGUI,
                                      thick);
  ImGui::GetWindowDrawList()->AddLine(origin, positionAxis[2], BLUE_IMGUI,
                                      thick);
  ImGui::GetWindowDrawList()->AddCircleFilled(origin, radius, WHITE_IMGUI);
  ImGui::GetWindowDrawList()->AddCircleFilled(positionAxis[0], radius,
                                              RED_IMGUI);
  ImGui::GetWindowDrawList()->AddCircleFilled(positionAxis[1], radius,
                                              GREEN_IMGUI);
  ImGui::GetWindowDrawList()->AddCircleFilled(positionAxis[2], radius,
                                              BLUE_IMGUI);

  ImGui::GetWindowDrawList()->AddConvexPolyFilled(xyPlane, 4, BLUE_IMGUI);
  ImGui::GetWindowDrawList()->AddConvexPolyFilled(xzPlane, 4, GREEN_IMGUI);
  ImGui::GetWindowDrawList()->AddConvexPolyFilled(yzPlane, 4, RED_IMGUI);

  return isChanged;
}

bool SceneEditor::handleRotationSystem(glm::vec3 &rotation) {

  bool isChanged = false;
  float thick = 2.0f;
  float radius = 10.0f;

  ImVec2 origin = worldToScreen(mCoordSystem->mCenter.mModedPosition);
  ImVec2 rotationGrab[3];
  for (int i = 0; i < 3; i++) {
    rotationGrab[i] =
        worldToScreen(mCoordSystem->mRotationGrab[i].mModedPosition);
  }
  ImVec2 xR[RESOLUTION];
  ImVec2 yR[RESOLUTION];
  ImVec2 zR[RESOLUTION];
  for (int i = 0; i < RESOLUTION; i++) {
    xR[i] = worldToScreen(mCoordSystem->mRotX[i].mModedPosition);
    yR[i] = worldToScreen(mCoordSystem->mRotY[i].mModedPosition);
    zR[i] = worldToScreen(mCoordSystem->mRotZ[i].mModedPosition);
  }

  static ImVec2 startX, startY, startZ;

  if (!mRotatingX && !mRotatingY && !mRotatingZ &&
      ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    if (Distance(mousePos, rotationGrab[0]) <= radius) {
      mRotatingX = true;
      startX = mousePos;
    } else if (Distance(mousePos, rotationGrab[1]) <= radius) {
      mRotatingY = true;
      startY = mousePos;
    } else if (Distance(mousePos, rotationGrab[2]) <= radius) {
      mRotatingZ = true;
      startZ = mousePos;
    }
  }

  ImVec2 mousePos = ImGui::GetIO().MousePos;
  ImVec2 rotationVector = ImVec2(mousePos.x - origin.x, mousePos.y - origin.y);
  float currentRotationAngle = atan2(rotationVector.y, rotationVector.x);

  if (mRotatingX) {
    float initialRotationAngle =
        atan2(startX.y - origin.y, startX.x - origin.x);
    float rotationAngle = (currentRotationAngle - initialRotationAngle);
    rotation.x -= glm::degrees(rotationAngle);
    isChanged = true;
  } else if (mRotatingY) {
    float initialRotationAngleY =
        atan2(startY.y - origin.y, startY.x - origin.x);
    float rotationAngle = currentRotationAngle - initialRotationAngleY;
    rotation.y -= glm::degrees(rotationAngle);
    isChanged = true;
  } else if (mRotatingZ) {
    float initialRotationAngleZ =
        atan2(startZ.y - origin.y, startZ.x - origin.x);
    float rotationAngle = currentRotationAngle - initialRotationAngleZ;
    rotation.z -= glm::degrees(rotationAngle);
    isChanged = true;
  }
  if (isChanged) {
    startX = mousePos;
    startY = mousePos;
    startZ = mousePos;
  }
  if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
    mRotatingX = false;
    mRotatingY = false;
    mRotatingZ = false;
  }

  for (int i = 0; i < RESOLUTION - 1; i++) {
    ImGui::GetWindowDrawList()->AddLine(xR[i], xR[i + 1], RED_IMGUI, thick);
    ImGui::GetWindowDrawList()->AddLine(yR[i], yR[i + 1], GREEN_IMGUI, thick);
    ImGui::GetWindowDrawList()->AddLine(zR[i], zR[i + 1], BLUE_IMGUI, thick);
  }

  int last = RESOLUTION - 1;
  ImGui::GetWindowDrawList()->AddLine(xR[last], xR[0], RED_IMGUI, thick);
  ImGui::GetWindowDrawList()->AddLine(yR[last], yR[0], GREEN_IMGUI, thick);
  ImGui::GetWindowDrawList()->AddLine(zR[last], zR[0], BLUE_IMGUI, thick);
  ImGui::GetWindowDrawList()->AddCircleFilled(origin, radius, WHITE_IMGUI);
  ImGui::GetWindowDrawList()->AddCircleFilled(rotationGrab[0], radius,
                                              RED_IMGUI);
  ImGui::GetWindowDrawList()->AddCircleFilled(rotationGrab[1], radius,
                                              GREEN_IMGUI);
  ImGui::GetWindowDrawList()->AddCircleFilled(rotationGrab[2], radius,
                                              BLUE_IMGUI);

  return isChanged;
}

ImVec2 SceneEditor::worldToScreen(const glm::vec3 &worldPos) {
  glm::vec3 mFront = mCamera->getFront();
  glm::vec3 mUp = mCamera->getUp();
  glm::vec3 mPosition = mCamera->getPosition();
  float fov = mCamera->getFOV();
  float aspectRatio = mCamera->getAspectRatio();

  // Construct the view matrix
  glm::mat4 viewMatrix = glm::lookAt(mPosition, mPosition + mFront, mUp);

  // Construct the projection matrix
  glm::mat4 projectionMatrix =
      glm::perspective(glm::radians(fov), aspectRatio, 0.01f, 1000.0f);

  // Combine the view and projection matrices
  glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;

  // Transform the 3D world position to screen space
  glm::vec4 screenPos = viewProjectionMatrix * glm::vec4(worldPos, 1.0f);

  // Out of view
  if (screenPos.z < 0.0f)
    return ImVec2(-10, -10);
  // Convert to normalized device coordinates
  screenPos /= screenPos.w;
  // Convert to ImGui screen space
  ImVec2 screenPoint(screenPos.x * 0.5f + 0.5f, -screenPos.y * 0.5f + 0.5f);

  // Convert to ImGui coordinates
  ImVec2 screenSize = ImGui::GetIO().DisplaySize;
  screenPoint.x *= screenSize.x;
  screenPoint.y *= screenSize.y;

  return screenPoint;
}
