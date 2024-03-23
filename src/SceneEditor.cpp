#include "SceneEditor.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <filesystem>
#include <string>
namespace fs = std::filesystem;

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
  bool change1 = ImGui::SliderFloat(labels[0], &values[0], min, max);
  bool change2 = ImGui::SliderFloat(labels[1], &values[1], min, max);
  bool change3 = ImGui::SliderFloat(labels[2], &values[2], min, max);
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
    : mModelsFolder(modelsFolder), mCamera(camera), mSettings(settings) {
  mScene = scene;

  refreshAvailableModels();
  refreshLoadedModels();
  refreshLoadedLights();

  mSelectedModelIndex = 0;
  mSelectedModel = mScene->getModel(mSelectedModelIndex);
  mSelectedLightIndex = 0;
  mSelectedLight = mScene->getLight(mSelectedLightIndex);
}

ChangeType SceneEditor::render(float fps, int dataSize) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ChangeType debugChange = debugWindow(fps, dataSize);
  ChangeType modelChange = modelWindow();
  ChangeType cameraChange = cameraWindow();
  ChangeType lightsChange = lightsWindow();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  if (!debugChange && !modelChange && !cameraChange && !lightsChange)
    return ChangeType::NoneType;
  if (debugChange == ChangeType::BVHType || modelChange == ChangeType::BVHType)
    return ChangeType::BVHType;
  if (debugChange == ChangeType::SettingsType)
    return ChangeType::SettingsType;
  if (modelChange == ChangeType::MaterialType)
    return ChangeType::MaterialType;
  if (lightsChange == ChangeType::LightType)
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
      editVec3("Rotate", mSelectedModel->modRotation(), labels, 0, 360);
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
      editVec3("Position", mSelectedModel->modPosition(), labels, -5, 5);
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
      }
    }
    ImGui::EndListBox();
  }
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
      }
    }
    ImGui::EndListBox();
  }
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
