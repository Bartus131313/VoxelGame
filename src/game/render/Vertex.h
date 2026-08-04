#pragma once

/** @brief Represents a single 2D vertex containing spatial position and texture coordinates. */
struct Vertex2D {
    float x{}, y{};     ///< Spatial position coordinates in 2D space.
    float u{}, v{};     ///< Texture mapping coordinates (UVs).

    /** @brief Configures OpenGL vertex attribute pointers for Vertex2D. */
    static void setupAttributes() {
        // Attribute 0: Position (vec2)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), reinterpret_cast<void*>(offsetof(Vertex2D, x)));

        // Attribute 1: Texture Coordinates (vec2)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), reinterpret_cast<void*>(offsetof(Vertex2D, u)));
    }
};

/**
 * @brief Represents a single 3D vertex containing spatial position,
 * texture coordinates, and normal vectors.
 */
struct Vertex3D {
    float x{}, y{}, z{};     ///< Spatial position coordinates in 3D space.
    float u{}, v{};          ///< Texture mapping coordinates (UVs).
    float nx{}, ny{}, nz{};  ///< Surface normal vector components used for lighting calculations.

    /** @brief Configures OpenGL vertex attribute pointers for Vertex3D. */
    static void setupAttributes() {
        // Attribute 0: Position (vec3)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), reinterpret_cast<void*>(offsetof(Vertex3D, x)));

        // Attribute 1: Texture Coordinates (vec2)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), reinterpret_cast<void*>(offsetof(Vertex3D, u)));

        // Attribute 2: Surface Normals (vec3)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), reinterpret_cast<void*>(offsetof(Vertex3D, nx)));
    }
};