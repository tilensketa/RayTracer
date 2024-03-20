#include "SceneEditor.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <filesystem>
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
  if (isChanged)
    return true;
  return false;
}

bool SceneEditor::editVec3(const char *text, glm::vec3 &vec,
                           const char *labels[3], float rangeMin,
                           float rangeMax) {
  bool isChanged = false;
  float values[3] = {vec.x, vec.y, vec.z};
  ImGui::Text("%s", text);
  ImGui::PushID(rangeMin * rangeMax);
  if (ImGui::SliderFloat(labels[0], &values[0], rangeMin, rangeMax)) {
    isChanged = true;
  }
  if (ImGui::SliderFloat(labels[1], &values[1], rangeMin, rangeMax)) {
    isChanged = true;
  }
  if (ImGui::SliderFloat(labels[2], &values[2], rangeMin, rangeMax)) {
    isChanged = true;
  }
  ImGui::PopID();
  if (isChanged) {
    vec.x = values[0];
    vec.y = values[1];
    vec.z = values[2];
  }
  return isChanged;
}

bool SceneEditor::editSliderFloat(const char *label, float &value, float min,
                                  float max, int index) {
  bool isChanged = false;
  ImGui::Text("%s", label);
  ImGui::PushItemWidth(-1);
  ImGui::PushID(index);
  if (ImGui::SliderFloat("##", &value, min, max)) {
    isChanged = true;
  }
  ImGui::PopID();
  return isChanged;
}

SceneEditor::SceneEditor(const std::string &modelsFolder,
                         std::shared_ptr<Scene> scene,
                         std::shared_ptr<Camera> camera,
                         std::shared_ptr<Settings> settings)
    : mModelsFolder(modelsFolder), mCamera(camera), mSettings(settings) {
  mScene = scene;
  for (const auto &entry : fs::directory_iterator(mModelsFolder)) {
    std::string extension = entry.path().extension().string();
    if (extension == ".obj" || extension == ".fbx") {
      std::string filename = entry.path().filename().string();
      mModelList.push_back(filename);
    }
  }
  refreshLoadedModels();

  mSelectedModelIndex = 0;
  mSelectedModel = mScene->getModel(mSelectedModelIndex);
}

bool SceneEditor::render(float fps, int dataSize) {
  bool isChanged = false;
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  if (debugWindow(fps, dataSize))
    isChanged = true;
  if (modelWindow())
    isChanged = true;
  if (editCamera())
    isChanged = true;

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  return isChanged;
}

bool SceneEditor::debugWindow(float fps, int dataSize) {
  bool isChanged = false;
  ImGui::Begin("Info");
  ImGui::Text("FPS: %f", fps);
  ImGui::Text("Models: %i", mScene->getModelCount());
  ImGui::Text("Triangles: %i", mScene->getTrianglesCount());
  ImGui::Text("Vertices: %i", mScene->getVerticesCount());
  ImGui::Text("Materials: %i", mScene->getMaterialsCount());
  ImGui::Text("Data: %i", dataSize);
  ImGui::Text("BVH depth:");
  ImGui::PushItemWidth(-1);
  if (ImGui::SliderInt("##BVH", &mSettings->mMaxDepth, 0, 30))
    isChanged = true;
  if (ImGui::Button(
          ("Mode: " + std::string(mSettings->mBlack ? "FLAT" : "SHADED"))
              .c_str())) {
    mSettings->mBlack = !mSettings->mBlack;
    isChanged = true;
  }
  ImGui::Text("Selected model:");
  if (mSelectedModel != nullptr)
    ImGui::Text("%s", mSelectedModel->getName().c_str());
  else
    ImGui::Text("None");
  ImGui::End();
  return isChanged;
}

bool SceneEditor::modelWindow() {
  bool isChanged = false;
  ImGui::Begin("Scene");
  if (loadModel())
    isChanged = true;
  ImGui::SameLine();
  if (removeModel())
    isChanged = true;
  modelSelector();
  if (transformModel())
    isChanged = true;
  if (materialEditor())
    isChanged = true;

  ImGui::End();
  return isChanged;
}

bool SceneEditor::loadModel() {
  bool isChanged = false;
  if (ImGui::Button("Load model")) {
    mShowLoadModelPopup = true;
  }

  if (mShowLoadModelPopup) {
    ImGui::OpenPopup("LoadModelPopup");

    if (ImGui::BeginPopup("LoadModelPopup")) {
      ImGui::Text("Select model");
      if (ImGui::BeginCombo("##selectModel",
                            mModelList[mSelectedLoadModelIndex].c_str())) {
        for (int i = 0; i < mModelList.size(); ++i) {
          bool isSelected = (mSelectedLoadModelIndex == i);
          if (ImGui::Selectable(mModelList[i].c_str(), isSelected))
            mSelectedLoadModelIndex = i;

          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      if (ImGui::Button("Load")) {
        std::string selectedModelName =
            mModelsFolder + mModelList[mSelectedLoadModelIndex];
        mScene->add(selectedModelName);
        if (mSelectedModel == nullptr) {
          mSelectedModelIndex = 0;
          mSelectedModel = mScene->getModel(mSelectedModelIndex);
        } else {
          mSelectedModelIndex = mLoadedModelList.size();
          mSelectedModel = mScene->getModel(mSelectedModelIndex);
        }
        isChanged = true;
        mShowLoadModelPopup = false;
        refreshLoadedModels();
      }
      ImGui::SameLine();
      if (ImGui::Button("Cancel")) {
        mShowLoadModelPopup = false;
      }
      ImGui::EndPopup();
    }
  }
  return isChanged;
}

bool SceneEditor::removeModel() {
  bool isChanged = false;
  if (ImGui::Button("Remove model")) {
    if (mSelectedModel == nullptr)
      return false;
    mScene->remove(mSelectedModel->getName());
    isChanged = true;
    mSelectedModel = nullptr;
    mSelectedModelIndex = -1;
    refreshLoadedModels();
  }
  return isChanged;
}

bool SceneEditor::transformModel() {
  bool isChanged = false;
  if (ImGui::CollapsingHeader("Transform")) {
    if (translateModel())
      isChanged = true;
    if (scaleModel())
      isChanged = true;
    if (rotateModel())
      isChanged = true;
  }
  return isChanged;
}

bool SceneEditor::scaleModel() {
  bool isChanged = false;
  if (mSelectedModel == nullptr)
    return false;

  const char *labels[3] = {"X", "Y", "Z"};
  if (editVec3("Scale", mSelectedModel->modScale(), labels, -2, 2))
    isChanged = true;
  if (isChanged) {
    mSelectedModel->update();
    mScene->recalculate();
  }
  return isChanged;
}

bool SceneEditor::rotateModel() {
  bool isChanged = false;
  if (mSelectedModel == nullptr)
    return false;

  const char *labels[3] = {"X", "Y", "Z"};
  if (editVec3("Rotate", mSelectedModel->modRotation(), labels, 0, 360))
    isChanged = true;
  if (isChanged) {
    mSelectedModel->update();
    mScene->recalculate();
  }
  return isChanged;
}

bool SceneEditor::translateModel() {
  bool isChanged = false;
  if (mSelectedModel == nullptr)
    return false;

  const char *labels[3] = {"X", "Y", "Z"};
  if (editVec3("Position", mSelectedModel->modPosition(), labels, -5, 5))
    isChanged = true;
  if (isChanged) {
    mSelectedModel->update();
    mScene->recalculate();
  }
  return isChanged;
}

void SceneEditor::modelSelector() {
  ImVec2 size(-1, 5 * ImGui::GetTextLineHeightWithSpacing());
  if (ImGui::BeginListBox("LoadedModels", size)) {
    for (int i = 0; i < mLoadedModelList.size(); ++i) {
      if (ImGui::Selectable(mLoadedModelList[i], mSelectedModelIndex == i)) {
        mSelectedModelIndex = i;
        mSelectedModel = mScene->getModel(i);
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

bool SceneEditor::editCamera() {
  ImGui::Begin("Camera");
  /* ImGui::Text("FOV");
  ImGui::PushItemWidth(-1);
  if (ImGui::SliderFloat("##FOV", &mCamera->modFOV(), 30, 90)) {
    isChanged = true;
  }
  ImGui::Text("Move Speed");
  ImGui::PushItemWidth(-1);
  if (ImGui::SliderFloat("##MoveSpeed", &mCamera->modMoveSpeed(), 0, 10)) {
    isChanged = true;
  }
  ImGui::Text("Rotate Speed");
  ImGui::PushItemWidth(-1);
  if (ImGui::SliderFloat("##RotateSpeed", &mCamera->modRotateSpeed(), 0, 2)) {
    isChanged = true;
  } */

  bool fovChanged = editSliderFloat("FOV", mCamera->modFOV(), 30, 90, 1);
  bool moveChanged =
      editSliderFloat("Move Speed", mCamera->modMoveSpeed(), 0, 10, 2);
  bool rotateChanged =
      editSliderFloat("Rotate Speed", mCamera->modRotateSpeed(), 0, 2, 3);

  ImGui::End();
  return (fovChanged || moveChanged || rotateChanged);
}

void SceneEditor::refreshLoadedModels() {
  mLoadedModelList.clear();
  for (const Model &model : mScene->getModels()) {
    mLoadedModelList.push_back(model.getName().c_str());
  }
}
