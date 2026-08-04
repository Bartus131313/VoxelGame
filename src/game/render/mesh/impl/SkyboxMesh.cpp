#include "SkyboxMesh.h"

SkyboxMesh::SkyboxMesh() {
    setupBuffers();
}

SkyboxMesh::~SkyboxMesh() {
    cleanup();
}

SkyboxMesh::SkyboxMesh(SkyboxMesh&& other) noexcept : Mesh(std::move(other)) {
    *this = std::move(other);
}

SkyboxMesh& SkyboxMesh::operator=(SkyboxMesh&& other) noexcept {
    if (this != &other) {
        cleanup();
        m_vao = std::exchange(other.m_vao, 0);
        m_vbo = std::exchange(other.m_vbo, 0);
    }
    return *this;
}

void SkyboxMesh::setupBuffers() {
    // 36 positions representing 12 triangles of a unit cube [-1, 1]
    static constexpr float skyboxVertices[] = {
        // Positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    // Attribute 0: Position (3 floats)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

void SkyboxMesh::bind() const {
    glBindVertexArray(m_vao);
}

void SkyboxMesh::unbind() const {
    glBindVertexArray(0);
}

void SkyboxMesh::render() const {
    if (m_vao == 0) return;

    glDisable(GL_CULL_FACE);

    // Change depth function so skybox passes depth test at maximum depth (z = 1.0)
    glDepthFunc(GL_LEQUAL);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // Reset depth function to default
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
}

void SkyboxMesh::cleanup() {
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_vao) glDeleteVertexArrays(1, &m_vao);

    m_vbo = 0;
    m_vao = 0;
}