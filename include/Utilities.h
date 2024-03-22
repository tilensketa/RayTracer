#pragma once

#include <string>

namespace Utils {

inline std::string extractFilename(const std::string &filePath) {
  size_t pos = filePath.find_last_of("/");
  if (pos != std::string::npos) {
    return filePath.substr(pos + 1);
  } else {
    return filePath;
  }
}

} // namespace Utils
