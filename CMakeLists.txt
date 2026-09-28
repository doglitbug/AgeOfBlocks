cmake_minimum_required(VERSION 3.22)
project(AgeOfBlocks VERSION 0.0.1 LANGUAGES C CXX)

# Force C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find System Dependencies
find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui)
set(IMGUI_SRCS
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        # Include the specific SDL3 and OpenGL3 backend implementations
        ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Define Executable
file(GLOB_RECURSE SOURCES
        "src/*.cpp"
        "src/*.h"
        "src/States/*.cpp"
        "src/States/*.h"
        "src/Subsystems*.cpp"
        "src/Subsystems*.h"
        "src/World*.cpp"
        "src/World*.h"
        "src/GUI*.cpp"
        "src/GUI*.h"
)
add_executable(${PROJECT_NAME}
        ${SOURCES}
        "include/glad/glad.c"
        ${IMGUI_SRCS}
)

# Include Directories
target_include_directories(${PROJECT_NAME} PRIVATE
        "include/"
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        "src"
        "src/Subsystems/"
        "src/World/"
        "src/GUI/"
        "src/States"
)

# Link Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
        SDL3::SDL3
        OpenGL::GL
        assimp::assimp
)
