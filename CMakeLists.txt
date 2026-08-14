cmake_minimum_required(VERSION 3.22)
project(AgeOfBlocks VERSION 0.0.1 LANGUAGES C CXX)

# Force C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find System Dependencies
find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)

# Define Executable
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "src/*.h"
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
)

# Include Directories
target_include_directories(${PROJECT_NAME} PRIVATE
    "include/"
    "src"
    "src/Subsystems/"
    "src/World/"
    "src/GUI/"
)

# Link Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3
    OpenGL::GL
    assimp::assimp
)
