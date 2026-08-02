cmake_minimum_required(VERSION 3.20)

# Enables CXX (C++) for your game and C for glad.c
project(VoxelGame CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build GLFW from thirdparty/glfw
# Disable building GLFW docs, tests, and examples to keep build fast
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(thirdparty/glfw)

# Define Executable & Source Files
add_executable(${PROJECT_NAME}
        src/main.cpp
        thirdparty/glad/src/glad.c
        src/core/window.cpp
        src/core/window.h
        src/input/keyboardManager.cpp
        src/input/KeyboardManager.h
        src/input/mouseManager.cpp
        src/input/MouseManager.h
        src/input/gamepadManager.cpp
        src/input/gamepadManager.h
        src/input/Input.h
        src/input/Input.cpp
        src/game/Game.cpp
        src/game/Game.h
        src/game/render/Shader.cpp
        src/game/render/Shader.h
        src/game/render/font/FontRenderer.cpp
        src/game/render/font/FontRenderer.h
        src/game/render/Vertex.h
        src/game/ui/UIElement.h
        src/game/ui/elements/UILabel.h
        src/game/ui/Canvas.h
        src/game/render/font/FontManager.cpp
        src/game/render/font/FontManager.h
        src/game/ui/Canvas.cpp
)

# Include Directories for Headers
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glad/include
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glm
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/stb
)

# Link Libraries
find_package(OpenGL REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw
        OpenGL::GL
)

# Copy assets folder
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)