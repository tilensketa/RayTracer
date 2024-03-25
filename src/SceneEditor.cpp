#include "SceneEditor.h"

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

bool SceneEditor::editVec3WithColorEdit3(const char *label, int index,
                                         glm::vec3 &vec) {
  bool isChanged = false;
  ImVec4 color(vec.x, vec.y, vec.z, 1.0f);
  ImGui::PushID(index);
  if (ImGui::ColorEdit3(label, (float *)&color)) {
    vec.x = color.x;
    vec.y = color.y;
    vec.z = color.z;
    isChanged = true;
  }
  ImGui::PopID();
  return isChanged;
}

bool SceneEditor::editVec3(const char *text, glm::vec3 &vec,
                           const char *labels[3], float min, float max) {
  float values[3] = {vec.x, vec.y, vec.z};
  ImGui::Text("%s", text);
  int index;
  if (min * max == 0) {
    if (min == 0)
      index = max;
    else
      index = min;
  } else
    index = min * max;
  ImGui::PushID(index);
  float speed = 0.1f;
  bool change1 = ImGui::DragFloat(labels[0], &values[0], speed, min, max);
  bool change2 = ImGui::DragFloat(labels[1], &values[1], speed, min, max);
  bool change3 = ImGui::DragFloat(labels[2], &values[2], speed, min, max);
  ImGui::PopID();
  bool isChanged = change1 || change2 || change3;
  if (isChanged) {
    vec.x = values[0];
    vec.y = values[1];
    vec.z = values[2];
  }
  return isChanged;
}

bool SceneEditor::editSliderFloat(const char *text, float &value, float min,
                                  float max) {
  ImGui::Text("%s", text);
  ImGui::PushItemWidth(-1);
  std::string id = "##";
  id += text;
  bool isChanged = ImGui::SliderFloat(id.c_str(), &value, min, max);
  return isChanged;
}
bool SceneEditor::editSliderInt(const char *text, int &value, int min,
                                int max) {
  ImGui::Text("%s", text);
  ImGui::PushItemWidth(-1);
  std::string id = "##";
  id += text;
  bool isChanged = ImGui::SliderInt(id.c_str(), &value, min, max);
  return isChanged;
}

SceneEditor::SceneEditor(const std::string &modelsFolder,
                         std::shared_ptr<Scene> scene,
                         std::shared_ptr<Camera> camera,
                         std::shared_ptr<Settings> settings)
    : mModelsFolder(modelsFolder), mScene(scene), mCamera(camera),
      mSettings(settings) {

  refreshAvailableModels();
  refreshLoadedModels();
  refreshLoadedLights();

  mSelectedModelIndex = 0;
  mSelectedModel = mScene->getModel(mSelectedModelIndex);
  /* mSelectedLightIndex = 0;
  mSelectedLight = mScene->getLight(mSelectedLightIndex); */

  mCoordSystem = std::make_unique<CoordinateSystem>();
}

ChangeType SceneEditor::render(float fps, int dataSize) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ChangeType debugChange = debugWindow(fps, dataSize);
  ChangeType modelChange = modelWindow();
  ChangeType cameraChange = cameraWindow();
  ChangeType lightsChange = lightsWindow();
  ChangeType overlayChange = overlayWindow();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  if (!debugChange && !modelChange && !cameraChange && !lightsChange &&
      !overlayChange)
    return ChangeType::NoneType;
  if (debugChange == ChangeType::BVHType ||
      modelChange == ChangeType::BVHType ||
      overlayChange == ChangeType::BVHType)
    return ChangeType::BVHType;
  if (debugChange == ChangeType::SettingsType)
    return ChangeType::SettingsType;
  if (modelChange == ChangeType::MaterialType)
    return ChangeType::MaterialType;
  if (lightsChange == ChangeType::LightType ||
      overlayChange == ChangeType::LightType)
    return ChangeType::LightType;
  if (cameraChange == ChangeType::CameraType)
    return ChangeType::CameraType;
  return ChangeType::NoneType;
}

ChangeType SceneEditor::debugWindow(float fps, int dataSize) {
  ImGui::Begin("DEBUG");
  ImGui::Text("FPS: %f", fps);
  ImGui::Text("Models: %i", mScene->getModelCount());
  ImGui::Text("Triangles: %i", mScene->getTrianglesCount());
  ImGui::Text("Vertices: %i", mScene->getVerticesCount());
  ImGui::Text("Materials: %i", mScene->getMaterialsCount());
  ImGui::Text("Data: %i", dataSize);
  ImGui::Text("BVH depth:");
  ImGui::PushItemWidth(-1);
  bool bvhChange = ImGui::SliderInt("##BVH", &mSettings->mMaxDepth, 0, 30);
  bool viewportModeChange = viewportTypeEdit();
  bool coordinateModeChange = coordinateSystemModeEdit();
  bool downsampleChange =
      editSliderInt("Downsample Factor", mSettings->mDownsampleFactor, 1, 20);

  viewSelected();

  ImGui::End();
  if (bvhChange)
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

ChangeType SceneEditor::modelWindow() {
  ImGui::Begin("Models");
  bool loadChange = loadModel();
  bool removeChange = removeModel();
  modelSelector();
  bool transformChange = transformModel();
  bool materialChange = materialEditor();
  ImGui::End();
  if (loadChange || removeChange || transformChange)
    return ChangeType::BVHType;
  else if (materialChange)
    return ChangeType::MaterialType;
  return ChangeType::NoneType;
}

bool SceneEditor::loadModel() {
  bool modelLoaded = false;
  if (ImGui::BeginMenu("Add")) {
    for (const std::string availableModelName : mAvailableModelList) {
      if (ImGui::MenuItem(availableModelName.c_str())) {
        mScene->addModel(mModelsFolder + availableModelName);
        modelLoaded = true;
      }
    }
    ImGui::EndMenu();
  }
  if (modelLoaded) {
    if (mSelectedModel == nullptr) {
      mSelectedModelIndex = 0;
    } else {
      mSelectedModelIndex = mLoadedModelList.size();
    }
    mSelectedModel =
        mScene->getModel(mScene->getModels()[mSelectedModelIndex].getIndex());
    refreshLoadedModels();

    deselectLight();
  }
  return modelLoaded;
}

bool SceneEditor::removeModel() {
  bool isChanged = false;
  if (ImGui::Button("Remove model")) {
    if (mSelectedModel == nullptr)
      return false;
    if (mScene->removeModel(mSelectedModelIndex)) {
      isChanged = true;
      mSelectedModel = nullptr;
      mSelectedModelIndex = -1;
      refreshLoadedModels();
    }
  }
  return isChanged;
}

bool SceneEditor::transformModel() {
  bool isChanged = false;
  if (ImGui::CollapsingHeader("Transform")) {
    bool translateChange = translateModel();
    bool scaleChange = scaleModel();
    bool rotateChange = rotateModel();
    if (translateChange || scaleChange || rotateChange)
      isChanged = true;
  }
  return isChanged;
}

bool SceneEditor::scaleModel() {
  bool isChanged = false;
  if (mSelectedModel == nullptr)
    return false;

  const char *labels[3] = {"X", "Y", "Z"};
  if (editVec3("Scale", mSelectedModel->modScale(), labels, 0.1f, 3))
    isChanged = true;
  if (isChanged) {
    mSelectedModel->update();
    mScene->recalculate();
  }
  return isChanged;
}

bool SceneEditor::rotateModel() {
  if (mSelectedModel == nullptr)
    return false;

  const char *labels[3] = {"X", "Y", "Z"};
  bool rotateChange =
      editVec3("Rotate", mSelectedModel->modRotation(), labels, -360, 360);
  if (rotateChange) {
    mSelectedModel->update();
    mScene->recalculate();
  }
  return rotateChange;
}

bool SceneEditor::translateModel() {
  if (mSelectedModel == nullptr)
    return false;

  const char *labels[3] = {"X", "Y", "Z"};
  bool translateChange =
      editVec3("Position", mSelectedModel->modPosition(), labels, -10, 10);
  if (translateChange) {
    mSelectedModel->update();
    mScene->recalculate();
  }
  return translateChange;
}

void SceneEditor::modelSelector() {
  ImVec2 size(-1, 5 * ImGui::GetTextLineHeightWithSpacing());
  if (ImGui::BeginListBox("LoadedModels", size)) {
    for (const Model &model : mScene->getModels()) {
      std::string name = model.getName();
      if (ImGui::Selectable(name.c_str(),
                            mSelectedModelIndex == model.getIndex())) {
        mSelectedModelIndex = model.getIndex();
        mSelectedModel = mScene->getModel(mSelectedModelIndex);

        deselectLight();
      }
    }
    ImGui::EndListBox();
  }
}

void SceneEditor::deselectModel() {
  mSelectedModelIndex = -1;
  mSelectedModel = nullptr;
}

bool SceneEditor::materialEditor() {
  bool isChanged = false;
  if (ImGui::CollapsingHeader("Material")) {
    if (albedoEdit())
      isChanged = true;
  }
  return isChanged;
}

bool SceneEditor::albedoEdit() {
  if (mSelectedModel == nullptr)
    return false;
  bool isChanged = false;
  ImGui::Text("Material");
  int index = 0;
  for (Mesh &mesh : mSelectedModel->modMeshes()) {
    if (editVec3WithColorEdit3("Diffuse", index,
                               mesh.modMaterial().modDiffuse())) {
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

  bool fovChanged = editSliderFloat("FOV", mCamera->modFOV(), 30, 90);
  bool moveChanged =
      editSliderFloat("Move Speed", mCamera->modMoveSpeed(), 0, 10);
  bool rotateChanged =
      editSliderFloat("Rotate Speed", mCamera->modRotateSpeed(), 0, 2);

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

void SceneEditor::lightSelector() {
  ImVec2 size(-1, 5 * ImGui::GetTextLineHeightWithSpacing());
  if (ImGui::BeginListBox("Lights", size)) {
    for (const Light &light : mScene->getLights()) {
      if (ImGui::Selectable(light.mName.c_str(),
                            mSelectedLightIndex == light.mIndex)) {
        mSelectedLightIndex = light.mIndex;
        mSelectedLight = mScene->getLight(mSelectedLightIndex);

        deselectModel();
      }
    }
    ImGui::EndListBox();
  }
}

void SceneEditor::deselectLight() {
  mSelectedLightIndex = -1;
  mSelectedLight = nullptr;
}

bool SceneEditor::lightEdit() {
  if (mSelectedLight == nullptr)
    return false;
  const char *labels[3] = {"X", "Y", "Z"};
  const char *constants[3] = {"Constant", "Linear", "Quadratic"};
  bool lightChanged = false;
  if (mSelectedLight->mType == LightType::Point) {
    bool positionChanged =
        editVec3("Position", mSelectedLight->mPosition, labels, -5, 5);
    ImGui::Text("Parameters");
    bool constantChanged =
        ImGui::SliderFloat("Constant", &mSelectedLight->mIntensity, 0, 1);
    bool linearChanged =
        ImGui::SliderFloat("Linear", &mSelectedLight->mPitch, 0, 1);
    bool quadraticChanged =
        ImGui::SliderFloat("Quadratic", &mSelectedLight->mYaw, 0, 1);
    bool colorChanged =
        editVec3WithColorEdit3("Color", 1234, mSelectedLight->mColor);
    if (positionChanged || constantChanged || linearChanged ||
        quadraticChanged || colorChanged)
      lightChanged = true;
  } else if (mSelectedLight->mType == LightType::Directional) {
    bool intensityChanged =
        ImGui::SliderFloat("Intensity", &mSelectedLight->mIntensity, 0, 5);
    bool pitchChanged =
        ImGui::SliderFloat("Pitch", &mSelectedLight->mPitch, 0, 360);
    bool yawChanged = ImGui::SliderFloat("Yaw", &mSelectedLight->mYaw, 0, 360);
    bool colorChanged =
        editVec3WithColorEdit3("Color", 1234, mSelectedLight->mColor);
    if (intensityChanged || pitchChanged || yawChanged || colorChanged)
      lightChanged = true;
  }
  return lightChanged;
}

bool SceneEditor::loadLight() {
  bool lightLoaded = false;
  if (ImGui::BeginMenu("Add")) {
    if (ImGui::MenuItem("Point Light")) {
      mScene->addLight(LightType::Point);
      lightLoaded = true;
    }
    if (ImGui::MenuItem("Directional Light")) {
      mScene->addLight(LightType::Directional);
      lightLoaded = true;
    }
    ImGui::EndMenu();
  }
  if (lightLoaded) {
    if (mSelectedLight == nullptr) {
      mSelectedLightIndex = 0;
    } else {
      mSelectedLightIndex = mLoadedLightsList.size();
    }
    mSelectedLight =
        mScene->getLight(mScene->getLights()[mSelectedLightIndex].mIndex);
    refreshLoadedLights();

    deselectModel();
  }
  return lightLoaded;
}

bool SceneEditor::removeLight() {
  bool lightRemoved = false;
  if (ImGui::Button("Remove light")) {
    if (mSelectedLight == nullptr)
      return false;
    if (mScene->removeLight(mSelectedLight->mIndex)) {
      lightRemoved = true;
      mSelectedLight = nullptr;
      mSelectedLightIndex = -1;
      refreshLoadedLights();
    }
  }
  return lightRemoved;
}

ChangeType SceneEditor::lightsWindow() {
  ImGui::Begin("Lights");
  bool lightLoaded = loadLight();
  bool lightRemoved = removeLight();
  lightSelector();
  bool lightChange = lightEdit();
  ImGui::End();
  if (lightChange || lightLoaded || lightRemoved)
    return ChangeType::LightType;
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
    if (mSelectedLight->mType == LightType::Point) {
      mCoordSystem->mMode = Mode::Position;
      if (drawCoordinateSystem(mSelectedLight->mPosition,
                               mSelectedLight->mColor,
                               mSelectedLight->mColor)) {
        lightChanged = true;
      }
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
  ImVec2 xA = worldToScreen(mCoordSystem->mAxisX.mModedPosition);
  ImVec2 yA = worldToScreen(mCoordSystem->mAxisY.mModedPosition);
  ImVec2 zA = worldToScreen(mCoordSystem->mAxisZ.mModedPosition);

  static ImVec2 startX, startY, startZ;

  // Check if mouse is pressed within the coordinate system's X-axis
  if (!mDraggingX && !mDraggingY && !mDraggingZ &&
      ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    if (Distance(mousePos, xA) <= radius) {
      mDraggingX = true;
      startX = mousePos;
    } else if (Distance(mousePos, yA) <= radius) {
      mDraggingY = true;
      startY = mousePos;
    } else if (Distance(mousePos, zA) <= radius) {
      mDraggingZ = true;
      startZ = mousePos;
    }
  }
  // If dragging in X-axis, update position of the coordinate system
  if (mDraggingX) {
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    float deltaPixels = mousePos.x - startX.x;
    float deltaX =
        deltaPixels *
        (mCoordSystem->mPosition.x - mCoordSystem->mAxisX.mModedPosition.x) /
        (xA.x - origin.x);
    scale.x -= deltaX;
    startX = mousePos;
    isChanged = true;
  }
  // If dragging in Y-axis, update position of the coordinate system
  else if (mDraggingY) {
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    float deltaPixels = mousePos.y - startY.y;
    float deltaY =
        deltaPixels *
        (mCoordSystem->mPosition.y - mCoordSystem->mAxisY.mModedPosition.y) /
        (yA.y - origin.y);
    scale.y -= deltaY;
    startY = mousePos;
    isChanged = true;
  }
  // If dragging in Z-axis, update position of the coordinate system
  else if (mDraggingZ) {
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    float deltaPixels = mousePos.x - startZ.x;
    float deltaZ =
        deltaPixels *
        (mCoordSystem->mPosition.z - mCoordSystem->mAxisZ.mModedPosition.z) /
        (zA.x - origin.x);
    scale.z -= deltaZ;
    startZ = mousePos;
    isChanged = true;
  }

  if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
    mDraggingX = false;
    mDraggingY = false;
    mDraggingZ = false;
  }

  ImGui::GetWindowDrawList()->AddLine(origin, xA, RED_IMGUI, thick);
  ImGui::GetWindowDrawList()->AddLine(origin, yA, GREEN_IMGUI, thick);
  ImGui::GetWindowDrawList()->AddLine(origin, zA, BLUE_IMGUI, thick);
  ImGui::GetWindowDrawList()->AddCircleFilled(origin, radius, WHITE_IMGUI);
  ImGui::GetWindowDrawList()->AddCircleFilled(xA, radius, RED_IMGUI);
  ImGui::GetWindowDrawList()->AddCircleFilled(yA, radius, GREEN_IMGUI);
  ImGui::GetWindowDrawList()->AddCircleFilled(zA, radius, BLUE_IMGUI);

  return isChanged;
}

bool SceneEditor::handlePositionSystem(glm::vec3 &position) {
  bool isChanged = false;
  float thick = 2.0f;
  float radius = 10.0f;

  ImVec2 origin = worldToScreen(mCoordSystem->mCenter.mModedPosition);
  ImVec2 xA = worldToScreen(mCoordSystem->mAxisX.mModedPosition);
  ImVec2 yA = worldToScreen(mCoordSystem->mAxisY.mModedPosition);
  ImVec2 zA = worldToScreen(mCoordSystem->mAxisZ.mModedPosition);

  static ImVec2 startX, startY, startZ;

  // Check if mouse is pressed within the coordinate system's X-axis
  if (!mDraggingX && !mDraggingY && !mDraggingZ &&
      ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    if (Distance(mousePos, xA) <= radius) {
      mDraggingX = true;
      startX = mousePos;
    } else if (Distance(mousePos, yA) <= radius) {
      mDraggingY = true;
      startY = mousePos;
    } else if (Distance(mousePos, zA) <= radius) {
      mDraggingZ = true;
      startZ = mousePos;
    }
  }
  // If dragging in X-axis, update position of the coordinate system
  if (mDraggingX) {
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    float deltaPixels = mousePos.x - startX.x;
    float deltaX = deltaPixels *
                   (position.x - mCoordSystem->mAxisX.mModedPosition.x) /
                   (xA.x - origin.x);
    position.x -= deltaX;
    startX = mousePos;
    isChanged = true;
  }
  // If dragging in Y-axis, update position of the coordinate system
  else if (mDraggingY) {
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    float deltaPixels = mousePos.y - startY.y;
    float deltaY = deltaPixels *
                   (position.y - mCoordSystem->mAxisY.mModedPosition.y) /
                   (yA.y - origin.y);
    position.y -= deltaY;
    startY = mousePos;
    isChanged = true;
  }
  // If dragging in Z-axis, update position of the coordinate system
  else if (mDraggingZ) {
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    float deltaPixels = mousePos.x - startZ.x;
    float deltaZ = deltaPixels *
                   (position.z - mCoordSystem->mAxisZ.mModedPosition.z) /
                   (zA.x - origin.x);
    position.z -= deltaZ;
    startZ = mousePos;
    isChanged = true;
  }

  if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
    mDraggingX = false;
    mDraggingY = false;
    mDraggingZ = false;
  }

  ImGui::GetWindowDrawList()->AddLine(origin, xA, RED_IMGUI, thick);
  ImGui::GetWindowDrawList()->AddLine(origin, yA, GREEN_IMGUI, thick);
  ImGui::GetWindowDrawList()->AddLine(origin, zA, BLUE_IMGUI, thick);
  ImGui::GetWindowDrawList()->AddCircleFilled(origin, radius, WHITE_IMGUI);
  ImGui::GetWindowDrawList()->AddCircleFilled(xA, radius, RED_IMGUI);
  ImGui::GetWindowDrawList()->AddCircleFilled(yA, radius, GREEN_IMGUI);
  ImGui::GetWindowDrawList()->AddCircleFilled(zA, radius, BLUE_IMGUI);

  return isChanged;
}

bool SceneEditor::handleRotationSystem(glm::vec3 &rotation) {

  bool isChanged = false;
  float thick = 2.0f;
  float radius = 10.0f;

  ImVec2 xR[RESOLUTION];
  ImVec2 yR[RESOLUTION];
  ImVec2 zR[RESOLUTION];
  ImVec2 grabX = worldToScreen(mCoordSystem->mGrabX.mModedPosition);
  ImVec2 grabY = worldToScreen(mCoordSystem->mGrabY.mModedPosition);
  ImVec2 grabZ = worldToScreen(mCoordSystem->mGrabZ.mModedPosition);
  ImVec2 origin = worldToScreen(mCoordSystem->mCenter.mModedPosition);

  for (int i = 0; i < RESOLUTION; i++) {
    xR[i] = worldToScreen(mCoordSystem->mRotX[i].mModedPosition);
    yR[i] = worldToScreen(mCoordSystem->mRotY[i].mModedPosition);
    zR[i] = worldToScreen(mCoordSystem->mRotZ[i].mModedPosition);
  }

  static ImVec2 startX, startY, startZ;

  if (!mRotatingX && !mRotatingY && !mRotatingZ &&
      ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    if (Distance(mousePos, grabX) <= radius) {
      mRotatingX = true;
      startX = mousePos;
    } else if (Distance(mousePos, grabY) <= radius) {
      mRotatingY = true;
      startY = mousePos;
    } else if (Distance(mousePos, grabZ) <= radius) {
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
  ImGui::GetWindowDrawList()->AddCircleFilled(grabX, radius, RED_IMGUI);
  ImGui::GetWindowDrawList()->AddCircleFilled(grabY, radius, GREEN_IMGUI);
  ImGui::GetWindowDrawList()->AddCircleFilled(grabZ, radius, BLUE_IMGUI);

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
