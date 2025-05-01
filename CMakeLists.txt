cmake_minimum_required(VERSION 3.10)
project(TerrainGenerator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)  # Add this line

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${GLEW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}  # Add this line
)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create executable
add_executable(TerrainGenerator ${SOURCES})

# Link libraries
target_link_libraries(TerrainGenerator
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    glfw
)