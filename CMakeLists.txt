cmake_minimum_required(VERSION 3.10)

project(RayTracer)

# Use FetchContent to download and include GLM
include(FetchContent)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0.9.9.8
    GIT_SHALLOW true
)

FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
    FetchContent_Populate(glm)
    add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
endif()

# Use FetchContent to download and include GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.4
)

FetchContent_GetProperties(glfw)
if(NOT glfw_POPULATED)
    FetchContent_Populate(glfw)
    add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
endif()

# Use FetchContent to download and include GLAD
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v0.1.34
)

FetchContent_GetProperties(glad)
if(NOT glad_POPULATED)
    FetchContent_Populate(glad)
    add_subdirectory(${glad_SOURCE_DIR} ${glad_BINARY_DIR})
endif()

# Use FetchContent to download and include Assimp
FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG v5.0.1
)

FetchContent_GetProperties(assimp)
if(NOT assimp_POPULATED)
  FetchContent_Populate(assimp)
  add_subdirectory(${assimp_SOURCE_DIR} ${assimp_BINARY_DIR})
endif()

# Download stb_image_write.h during configuration
file(DOWNLOAD "https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h"
              "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/stb/stb_image_write.h"
              STATUS stb_download_status)

# Check if the download was successful
if(NOT stb_download_status EQUAL 0)
    message(FATAL_ERROR "Failed to download stb_image_write.h: ${stb_download_status}")
endif()

# Download ImGui source files during configuration
file(DOWNLOAD "https://raw.githubusercontent.com/ocornut/imgui/master/imconfig.h"
              "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui/imconfig.h"
              STATUS imgui_download_status_1)
file(DOWNLOAD "https://raw.githubusercontent.com/ocornut/imgui/master/imgui.cpp"
              "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui/imgui.cpp"
              STATUS imgui_download_status_2)
file(DOWNLOAD "https://raw.githubusercontent.com/ocornut/imgui/master/imgui.h"
              "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui/imgui.h"
              STATUS imgui_download_status_3)
file(DOWNLOAD "https://raw.githubusercontent.com/ocornut/imgui/master/imgui_draw.cpp"
              "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui/imgui_draw.cpp"
              STATUS imgui_download_status_4)
file(DOWNLOAD "https://raw.githubusercontent.com/ocornut/imgui/master/imgui_internal.h"
              "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui/imgui_internal.h"
              STATUS imgui_download_status_5)
file(DOWNLOAD "https://raw.githubusercontent.com/ocornut/imgui/master/imgui_tables.cpp"
              "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui/imgui_tables.cpp"
              STATUS imgui_download_status_6)
file(DOWNLOAD "https://raw.githubusercontent.com/ocornut/imgui/master/imgui_widgets.cpp"
              "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui/imgui_widgets.cpp"
              STATUS imgui_download_status_7)
file(DOWNLOAD "https://raw.githubusercontent.com/ocornut/imgui/master/imstb_rectpack.h"
              "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui/imstb_rectpack.h"
              STATUS imgui_download_status_8)
file(DOWNLOAD "https://raw.githubusercontent.com/ocornut/imgui/master/imstb_textedit.h"
              "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui/imstb_textedit.h"
              STATUS imgui_download_status_9)
file(DOWNLOAD "https://raw.githubusercontent.com/ocornut/imgui/master/imstb_truetype.h"
              "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui/imstb_truetype.h"
              STATUS imgui_download_status_10)
file(DOWNLOAD "https://raw.githubusercontent.com/ocornut/imgui/master/backends/imgui_impl_glfw.h"
              "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui/imgui_impl_glfw.h"
              STATUS imgui_download_status_11)
file(DOWNLOAD "https://raw.githubusercontent.com/ocornut/imgui/master/backends/imgui_impl_glfw.cpp"
              "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui/imgui_impl_glfw.cpp"
              STATUS imgui_download_status_12)
file(DOWNLOAD "https://raw.githubusercontent.com/ocornut/imgui/master/backends/imgui_impl_opengl3.h"
              "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui/imgui_impl_opengl3.h"
              STATUS imgui_download_status_13)
file(DOWNLOAD "https://raw.githubusercontent.com/ocornut/imgui/master/backends/imgui_impl_opengl3.cpp"
              "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui/imgui_impl_opengl3.cpp"
              STATUS imgui_download_status_14)
file(DOWNLOAD "https://raw.githubusercontent.com/ocornut/imgui/master/backends/imgui_impl_opengl3_loader.h"
              "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui/imgui_impl_opengl3_loader.h"
              STATUS imgui_download_status_15)

# Check if the downloads were successful
if(NOT imgui_download_status_1 EQUAL 0 OR
   NOT imgui_download_status_2 EQUAL 0 OR
   NOT imgui_download_status_3 EQUAL 0 OR
   NOT imgui_download_status_4 EQUAL 0 OR
   NOT imgui_download_status_5 EQUAL 0 OR
   NOT imgui_download_status_6 EQUAL 0 OR
   NOT imgui_download_status_7 EQUAL 0 OR
   NOT imgui_download_status_8 EQUAL 0 OR
   NOT imgui_download_status_9 EQUAL 0 OR
   NOT imgui_download_status_10 EQUAL 0 OR
   NOT imgui_download_status_11 EQUAL 0 OR
   NOT imgui_download_status_12 EQUAL 0 OR
   NOT imgui_download_status_13 EQUAL 0 OR
   NOT imgui_download_status_14 EQUAL 0 OR
   NOT imgui_download_status_15 EQUAL 0)
    message(FATAL_ERROR "Failed to download ImGui files.")
endif()

set(IMGUI_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui/imgui_impl_opengl3.cpp
)

# Find necessary packages
find_package(OpenGL REQUIRED)
set(OpenGL_GL_PREFERENCE "GLVND")  # Specify GLVND preference

# Set source files
set(SOURCES
    src/main.cpp
    src/Shader.cpp
    src/Quad.cpp
    src/UBO.cpp
    src/Camera.cpp
    src/Mesh.cpp
    src/Model.cpp
    src/Data.cpp
    src/BVHNode.cpp
    src/Application.cpp
    # Add other source files here if any
)

# Add executable
add_executable(RayTracer ${SOURCES} ${IMGUI_SOURCE_FILES})

# Include directories for OpenGL, GLFW, GLM, GLAD, Assimp, Imgui and stb
target_include_directories(RayTracer PRIVATE
    ${OPENGL_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${glm_SOURCE_DIR}
    ${glad_SOURCE_DIR}/include
    ${assimp_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/stb
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link against OpenGL, GLFW, GLM, GLAD ,Assimp, Imgui and stb libraries
target_link_libraries(RayTracer PRIVATE
    ${OPENGL_LIBRARIES}
    glfw
    ${glm_LIBRARY}
    glad
    assimp::assimp
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
