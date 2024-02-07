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
    # Add other source files here if any
)

# Add executable
add_executable(RayTracer ${SOURCES})

# Include directories for OpenGL, GLFW, GLM, and GLAD
target_include_directories(RayTracer PRIVATE
    ${OPENGL_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${glm_SOURCE_DIR}
    ${glad_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link against OpenGL, GLFW, GLM, and GLAD libraries
target_link_libraries(RayTracer PRIVATE
    ${OPENGL_LIBRARIES}
    glfw
    ${glm_LIBRARY}
    glad
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
