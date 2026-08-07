cmake_minimum_required(VERSION 3.20)

# Enables CXX (C++) for your game and C for glad.c
project(VoxelGame CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build GLFW from third_party/glfw
# Disable building GLFW docs, tests, and examples to keep build fast
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(third_party/glfw)

# Define Executable & Source Files
add_executable(${PROJECT_NAME}
        third_party/glad/src/glad.c
        src/main.cpp
        src/core/Window.cpp
        src/core/Window.h
        src/input/KeyboardManager.cpp
        src/input/KeyboardManager.h
        src/input/MouseManager.cpp
        src/input/MouseManager.h
        src/input/GamepadManager.cpp
        src/input/GamepadManager.h
        src/input/Input.h
        src/input/Input.cpp
        src/game/Game.cpp
        src/game/Game.h
        src/game/render/shader/Shader.cpp
        src/game/render/shader/Shader.h
        src/game/render/font/FontRenderer.cpp
        src/game/render/font/FontRenderer.h
        src/game/render/Vertex.h
        src/game/ui/UIElement.h
        src/game/ui/elements/UILabel.h
        src/game/ui/Canvas.h
        src/game/render/font/FontManager.cpp
        src/game/render/font/FontManager.h
        src/game/ui/Canvas.cpp
        src/game/render/shader/ShaderManager.cpp
        src/game/render/shader/ShaderManager.h
        src/game/render/camera/Camera3D.cpp
        src/game/render/camera/Camera3D.h
        src/game/entity/impl/Player.cpp
        src/game/entity/impl/Player.h
        src/core/IWindowEventListener.h
        src/game/render/mesh/Mesh.h
        src/game/render/mesh/impl/StaticMesh.cpp
        src/game/render/mesh/impl/StaticMesh.h
        src/game/render/mesh/impl/SkyboxMesh.cpp
        src/game/render/mesh/impl/SkyboxMesh.h
        src/game/render/sky/SkyRenderer.cpp
        src/game/render/sky/SkyRenderer.h
        src/game/render/RenderSystem.cpp
        src/game/render/RenderSystem.h
        src/game/entity/Entity.cpp
        src/game/entity/Entity.h
        src/core/Logger.cpp
        src/core/Logger.h
        src/game/resource/ResourceLocation.h
        src/game/voxel/chunk/SubChunkData.h
        src/game/voxel/block/BlockState.h
        src/game/voxel/chunk/ChunkData.h
        src/game/voxel/chunk/RegionData.h
        src/game/voxel/WorldConfig.h
        src/game/voxel/WorldData.cpp
        src/game/voxel/WorldData.h
        src/game/render/world/WorldRenderer.cpp
        src/game/render/world/WorldRenderer.h
        src/game/voxel/Direction.h
        src/game/render/texture/TextureAtlasManager.cpp
        src/game/render/texture/TextureAtlasManager.h
)

# Include Directories for Headers
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/nlohmann
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
        ${CMAKE_CURRENT_SOURCE_DIR}/res/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)

# Copy data folder
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/res/data
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/data
)