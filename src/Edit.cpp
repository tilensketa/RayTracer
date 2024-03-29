#include "Edit.h"

#include <imgui_internal.h>

bool Edit::colorEdit3(const std::string &label, glm::vec3 &vec) {
  bool isChanged = false;
  ImVec4 color(vec.x, vec.y, vec.z, 1.0f);
  ImGui::PushID(label.c_str());
  if (ImGui::ColorEdit3(label.c_str(), (float *)&color)) {
    vec.x = color.x;
    vec.y = color.y;
    vec.z = color.z;
    isChanged = true;
  }
  ImGui::PopID();
  return isChanged;
}

bool Edit::vec3(const std::string &label, glm::vec3 &value) {
  bool isChanged = false;

  ImGui::PushID(label.c_str());

  ImGui::Columns(2);
  ImGui::SetColumnWidth(0, 100);
  ImGui::Text("%s", label.c_str());
  ImGui::NextColumn();

  ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
  float lineHeight =
      GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
  ImVec2 buttonSize = ImVec2(lineHeight + 3.0f, lineHeight);

  // X
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9f, 0, 0, 1));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 0, 0, 1));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9f, 0, 0, 1));
  if (ImGui::Button("X", buttonSize)) {
    value.x = 0.0f;
    isChanged = true;
  }
  ImGui::PopStyleColor(3);
  ImGui::SameLine();
  if (ImGui::DragFloat("##X", &value.x, 0.1f, 0.0f, 0.0f, "%.2f"))
    isChanged = true;
  ImGui::PopItemWidth();
  ImGui::SameLine();

  // Y
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0.9f, 0, 1));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 1, 0, 1));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0.9f, 0, 1));
  if (ImGui::Button("Y", buttonSize)) {
    value.y = 0.0f;
    isChanged = true;
  }
  ImGui::PopStyleColor(3);
  ImGui::SameLine();
  if (ImGui::DragFloat("##Y", &value.y, 0.1f, 0.0f, 0.0f, "%.2f"))
    isChanged = true;
  ImGui::PopItemWidth();
  ImGui::SameLine();

  // Z
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0.9f, 1));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 1, 1));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0.9f, 1));
  if (ImGui::Button("Z", buttonSize)) {
    value.z = 0.0f;
    isChanged = true;
  }
  ImGui::PopStyleColor(3);
  ImGui::SameLine();
  if (ImGui::DragFloat("##Z", &value.z, 0.1f, 0.0f, 0.0f, "%.2f"))
    isChanged = true;
  ImGui::PopItemWidth();
  ImGui::SameLine();

  ImGui::PopStyleVar();
  ImGui::Columns(1);
  ImGui::PopID();

  return isChanged;
}

bool Edit::slider(const std::string &label, int &value, int min, int max) {
  ImGui::Columns(2);
  ImGui::SetColumnWidth(0, 100);

  ImGui::Text("%s", label.c_str());
  ImGui::NextColumn();

  ImGui::PushItemWidth(-1);
  std::string id = "##" + label;
  bool changed = ImGui::SliderInt(id.c_str(), &value, min, max);
  ImGui::PopItemWidth();

  ImGui::Columns(1);
  return changed;
}

bool Edit::slider(const std::string &label, float &value, float min,
                  float max) {
  ImGui::Columns(2);
  ImGui::SetColumnWidth(0, 100);

  ImGui::Text("%s", label.c_str());
  ImGui::NextColumn();

  ImGui::PushItemWidth(-1);
  std::string id = "##" + label;
  bool changed = ImGui::SliderFloat(id.c_str(), &value, min, max);
  ImGui::PopItemWidth();

  ImGui::Columns(1);
  return changed;
}
