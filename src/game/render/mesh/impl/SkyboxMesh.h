#pragma once

#include "../Mesh.h"
#include <glad/glad.h>
#include <utility>

/**
 * @brief Represents a static unit-cube mesh dedicated to rendering sky box.
 *
 * Inherits from Mesh and hardcodes a 36-vertex cube. Position vectors doubles as
 * sample coordinates for OpenGL Cubemaps (samplerCube).
 */
class SkyboxMesh : public Mesh {
public:
    SkyboxMesh();
    ~SkyboxMesh() override;

    // Move Operations
    SkyboxMesh(SkyboxMesh&& other) noexcept;
    SkyboxMesh& operator=(SkyboxMesh&& other) noexcept;

    void render() const override;
    void bind() const override;
    void unbind() const override;

    [[nodiscard]] std::size_t getCount() const override { return 36; }
    [[nodiscard]] unsigned int getVAO() const { return m_vao; }

private:
    void setupBuffers();
    void cleanup();

    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
};