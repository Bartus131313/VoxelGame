#pragma once

#include <glad/glad.h>
#include <vector>
#include <utility>

#include "../Mesh.h"
#include "../../Vertex.h"

/** @brief Concrete OpenGL mesh managing VAO, VBO, and EBO buffers. */
class StaticMesh : public Mesh {
public:
    /**
     * @brief Constructs GPU buffers from vertex and index vectors.
     *
     * @param vertices Array of vertex structures.
     * @param indices Array of element indices.
     */
    StaticMesh(const std::vector<Vertex3D>& vertices, const std::vector<unsigned int>& indices);

    ~StaticMesh() override { cleanup(); }

    // Move Constructor
    StaticMesh(StaticMesh&& other) noexcept : Mesh(std::move(other)) { *this = std::move(other); }

    // Move Assignment
    StaticMesh& operator=(StaticMesh&& other) noexcept {
        if (this != &other) {
            cleanup();

            m_vao = std::exchange(other.m_vao, 0);
            m_vbo = std::exchange(other.m_vbo, 0);
            m_ebo = std::exchange(other.m_ebo, 0);
            m_indexCount = std::exchange(other.m_indexCount, 0);
        }
        return *this;
    }

    /**
     * @brief Binds the underlying Vertex Array Object (VAO).
     */
    void bind() const override;

    /**
     * @brief Unbinds the current Vertex Array Object (VAO).
     */
    void unbind() const override;

    /**
     * @brief Binds the vertex array and issues the appropriate OpenGL draw call.
     */
    void render() const override;

    /**
     * @brief Gets the total number of indices or vertices to render.
     *
     * @return Element draw count.
     */
    [[nodiscard]] std::size_t getCount() const override { return m_indexCount; }

    /**
     * @brief Gets VAO used by this static mesh.
     *
     * @return Vertex Array Object.
     */
    [[nodiscard]] unsigned int getVAO() const { return m_vao; }

private:
    /**
     * @brief Uploads vertices to VBO and indices to VAO.
     *
     * @param vertexData All vertices of this mesh.
     * @param vertexSize Size of a single vertex.
     * @param indexData All indices used to create faces.
     * @param indexSize Size of a single index.
     */
    void setupBuffers(const void* vertexData, std::size_t vertexSize,
                      const void* indexData, std::size_t indexSize);

    /**
     * @brief Frees all resources used by this mesh.
     */
    void cleanup();

    GLuint m_vao = 0;               ///< OpenGL handle for the Vertex Array Object (VAO).
    GLuint m_vbo = 0;               ///< OpenGL handle for the Vertex Buffer Object (VBO) storing vertex data.
    GLuint m_ebo = 0;               ///< OpenGL handle for the Element Buffer Object (EBO) storing index data.
    std::size_t m_indexCount = 0;   ///< Total number of indices to draw during rendering passes.
};