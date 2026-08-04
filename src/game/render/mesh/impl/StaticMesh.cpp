#include "StaticMesh.h"

StaticMesh::StaticMesh(const std::vector<Vertex3D>& vertices, const std::vector<unsigned int>& indices)
        : m_indexCount(indices.size())
{
    setupBuffers(vertices.data(), vertices.size() * sizeof(Vertex3D),
                 indices.data(), indices.size() * sizeof(unsigned int));
}

void StaticMesh::bind() const {
    glBindVertexArray(m_vao);
}

void StaticMesh::unbind() const {
    glBindVertexArray(0);
}

void StaticMesh::render() const {
    if (m_vao == 0 || m_indexCount == 0) return;

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void StaticMesh::setupBuffers(const void* vertexData, const std::size_t vertexSize,
                      const void* indexData, const std::size_t indexSize)
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    // Upload Vertices
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexSize), vertexData, GL_STATIC_DRAW);

    // Upload Indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indexSize), indexData, GL_STATIC_DRAW);

    // Configure vertex attributes via Vertex3D::setupAttributes()
    if constexpr (requires { Vertex3D::setupAttributes(); }) {
        Vertex3D::setupAttributes();
    }

    glBindVertexArray(0);
}

void StaticMesh::cleanup() {
    if (m_ebo) glDeleteBuffers(1, &m_ebo);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_vao) glDeleteVertexArrays(1, &m_vao);

    m_vao = 0;
    m_vbo = 0;
    m_ebo = 0;
    m_indexCount = 0;
}