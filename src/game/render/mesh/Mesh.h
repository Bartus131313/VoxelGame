#pragma once

#include <cstddef>

/**
 * @brief Abstract interface representing a renderable geometric mesh in OpenGL.
 *
 * Provides a uniform rendering API for different mesh types (e.g., static models,
 * dynamic voxel chunk meshes, UI quads, or particle systems).
 */
class Mesh {
public:
    virtual ~Mesh() = default;

    // Prevent copying to enforce unique ownership of OpenGL GPU handles
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    // Allow move semantics for transferring GPU buffer ownership
    Mesh(Mesh&&) noexcept = default;
    Mesh& operator=(Mesh&&) noexcept = default;

    /**
     * @brief Binds the underlying Vertex Array Object (VAO).
     */
    virtual void bind() const = 0;

    /**
     * @brief Unbinds the current Vertex Array Object (VAO).
     */
    virtual void unbind() const = 0;

    /**
     * @brief Binds the vertex array and issues the appropriate OpenGL draw call.
     */
    virtual void render() const = 0;

    /**
     * @brief Gets the total number of indices or vertices to render.
     *
     * @return Element draw count.
     */
    [[nodiscard]] virtual std::size_t getCount() const = 0;

protected:
    Mesh() = default;
};