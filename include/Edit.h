#include <glm/glm.hpp>
#include <imgui.h>
#include <string>

namespace Edit {
bool colorEdit3(const std::string &label, glm::vec3 &vec);
bool vec3(const std::string &label, glm::vec3 &value);
bool slider(const std::string &label, int &value, int min, int max);
bool slider(const std::string &label, float &value, float min, float max);
} // namespace Edit
