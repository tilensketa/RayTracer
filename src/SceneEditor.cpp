#include "SceneEditor.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <filesystem>
namespace fs = std::filesystem;
#include <iostream>

bool SceneEditor::EditVec3WithColorEdit3(const char *label, glm::vec3 &vec) {
  ImVec4 color(vec.x, vec.y, vec.z, 1.0f);
  if (ImGui::ColorEdit3(label, (float *)&color)) {
    vec.x = color.x;
    vec.y = color.y;
    vec.z = color.z;
    return true;
  }
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
        std::string selectedModel =
            mModelsFolder + mModelList[mSelectedLoadModelIndex];
        Model loadedModel(selectedModel);
        mScene->add(loadedModel);
        if (mSelectedModel == nullptr)
          mSelectedModel = mScene->getModel(0);
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
    refreshLoadedModels();
  }
  return isChanged;
}

bool SceneEditor::translateModel() {
  bool isChanged = false;
  if (mSelectedModel == nullptr)
    return false;

  ImGui::Text("Position");
  if (ImGui::SliderFloat("X", &mSelectedModel->modPosition().x, -10, 10)) {
    isChanged = true;
  }
  if (ImGui::SliderFloat("Y", &mSelectedModel->modPosition().y, -10, 10)) {
    isChanged = true;
  }
  if (ImGui::SliderFloat("Z", &mSelectedModel->modPosition().z, -10, 10)) {
    isChanged = true;
  }
  if (isChanged) {
    // TODO translate model
  }
  return isChanged;
}

void SceneEditor::modelSelector() {
  if (ImGui::BeginListBox(
          "LoadedModels",
          ImVec2(-1, 5 * ImGui::GetTextLineHeightWithSpacing()))) {
    for (int i = 0; i < mLoadedModelList.size(); ++i) {
      if (ImGui::Selectable(mLoadedModelList[i],
                            mSelectedModel == mScene->getModel(i))) {
        mSelectedModel = mScene->getModel(i);
      }

      if (mSelectedModel == mScene->getModel(i) && ImGui::IsItemFocused()) {
        ImGui::SetItemDefaultFocus();
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
  for (Mesh &mesh : mSelectedModel->modMeshes()) {
    if (EditVec3WithColorEdit3("Diffuse", mesh.modMaterial().modDiffuse())) {
      isChanged = true;
      // TODO change color
    }
  }
  return isChanged;
}

void SceneEditor::refreshLoadedModels() {
  mLoadedModelList.clear();
  for (const Model &model : mScene->getModels()) {
    mLoadedModelList.push_back(model.getName().c_str());
  }
}
