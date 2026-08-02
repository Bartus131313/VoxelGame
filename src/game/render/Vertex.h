#pragma once

/// Represents a single 2D vertex containing spatial position and texture coordinates.
struct Vertex2D {
    float x{}, y{}; ///< Spatial position coordinates in 2D space.
    float u{}, v{}; ///< Texture mapping coordinates (UVs).
};

/// Represents a single 3D vertex containing spatial position, texture coordinates, and normal vectors.
struct Vertex3D {
    float x{}, y{}, z{};     ///< Spatial position coordinates in 3D space.
    float u{}, v{};          ///< Texture mapping coordinates (UVs).
    float nx{}, ny{}, nz{};  ///< Surface normal vector components used for lighting calculations.
};