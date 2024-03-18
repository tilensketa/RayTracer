#include "SceneEditor.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <filesystem>
namespace fs = std::filesystem;

bool SceneEditor::EditVec3WithColorEdit3(const char *label, int index,
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

SceneEditor::SceneEditor(const std::string &modelsFolder,
                         std::shared_ptr<Scene> scene,
                         std::shared_ptr<Settings> settings)
    : mModelsFolder(modelsFolder), mSettings(settings) {
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
          ("Mode: " + std::string(mSettings->mBlack ? "BLACK" : "COLOR"))
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
  if (translateModel())
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
      if (ImGui::BeginCombo("Select Model",
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
          mSelectedModel = mScene->getModel(0);
        } else {
          mSelectedModel = mScene->getModel(mLoadedModelList.size());
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

bool SceneEditor::translateModel() {
  bool isChanged = false;
  if (mSelectedModel == nullptr)
    return false;

  float position[3] = {mSelectedModel->getPosition().x,
                       mSelectedModel->getPosition().y,
                       mSelectedModel->getPosition().z};
  ImGui::Text("Position");
  float tRange = 5;
  if (ImGui::SliderFloat("X", &position[0], -tRange, tRange)) {
    isChanged = true;
  }
  if (ImGui::SliderFloat("Y", &position[1], -tRange, tRange)) {
    isChanged = true;
  }
  if (ImGui::SliderFloat("Z", &position[2], -tRange, tRange)) {
    isChanged = true;
  }
  if (isChanged) {
    mSelectedModel->setPosition({position[0], position[1], position[2]});
    mSelectedModel->updatePosition();
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
  if (mSelectedModel == nullptr)
    return false;
  bool isChanged = false;
  ImGui::Text("Material");
  int index = 0;
  for (Mesh &mesh : mSelectedModel->modMeshes()) {
    if (EditVec3WithColorEdit3("Diffuse", index,
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

void SceneEditor::refreshLoadedModels() {
  mLoadedModelList.clear();
  for (const Model &model : mScene->getModels()) {
    mLoadedModelList.push_back(model.getName().c_str());
  }
}
