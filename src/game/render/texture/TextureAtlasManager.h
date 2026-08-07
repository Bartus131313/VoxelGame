#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../resource/ResourceLocation.h"
#include "glad/glad.h"

/** @brief Represents the normalized UV coordinates (0.0 to 1.0). */
struct UVBox {
    float uMin, vMin;
    float uMax, vMax;
};

/** @brief Metadata specifically for tracking animated textures on the atlas. */
struct AnimatedTexture {
    int atlasX;       ///< Pixel X position on the giant atlas sheet
    int atlasY;       ///< Pixel Y position on the giant atlas sheet
    int tileWidth;    ///< Tile width (e.g., 16)
    int tileHeight;   ///< Tile height (e.g., 16)

    /// All frames of the animated textures loaded from disk.
    std::vector<std::vector<uint8_t>> frames;

    size_t currentFrame{0};         ///< Current frame of animation.
    float timeAccumulator{0.0f};    ///< Accumulated time in seconds.
    uint32_t frameDuration{3};      ///< Frame duration in game ticks.
};

/**
 * @brief Represents single atlas with all loaded textures.
 * (1 atlas = 1 directory in textures folder.)
 */
struct TextureAtlas {
    GLuint textureID;   ///< ID of OpenGL texture.
    int width;          ///< Total width of atlas in pixels.
    int height;         ///< Total height of atlas in pixels.

    /// Stores UV boxes of textures mapped by a unique string key.
    std::unordered_map<std::string, UVBox> uvMap;

    /// Stores animated textures mapped by a unique string key.
    std::unordered_map<std::string, AnimatedTexture> animMap;
};

class TextureAtlasManager {
public:
    /** @brief Loads all atlases from game's @c textures/ folder. */
    static void init();

    /**
     * @brief Updates all animated textures in all atlases.
     *
     * @param deltaTime Time elapsed since last frame.
     */
    static void update(float deltaTime);

    /** @brief Dumps a packed atlas texture to disk as a PNG for debugging. */
    static void saveAtlasFile(const std::string& atlasName, const std::string& outputPath);

    /**
     * @brief Gets UV box for specific texture in specific atlas.
     *
     * @param atlasName Name of the atlas.
     * @param textureName Name of the texture.
     *
     * @return UV box of the texture.
     */
    static UVBox getUVBox(const std::string& atlasName, const std::string& textureName);

    /** @brief Gets the OpenGL texture ID for a specific atlas sheet. */
    [[nodiscard]] static GLuint getAtlasTextureID(const std::string& atlasName);

    /** @brief Frees all resources (texture atlases) loaded by this manager. */
    static void cleanup();

private:
    static const ResourceLocation texturesLocation;

    /**
     * @brief Calculates proper UV box (0.0 to 1.0) based on raw pixel box.
     *
     * @param x X position of the box top-left corner.
     * @param y Y position of the box top-left corner.
     * @param width Width of the box.
     * @param height Height of the box.
     * @param atlasWidth Width of the atlas.
     * @param atlasHeight Height of the atlas.
     *
     * @return @c UVBox containing normalized uvs.
     */
    [[nodiscard]] static UVBox calculateUVs(const int x, const int y, const int width, const int height,
        const int atlasWidth, const int atlasHeight) {
        return {
            static_cast<float>(x) / static_cast<float>(atlasWidth),
            static_cast<float>(y) / static_cast<float>(atlasHeight),
            static_cast<float>(x + width) / static_cast<float>(atlasWidth),
            static_cast<float>(y + height) / static_cast<float>(atlasHeight)
        };
    }

    /// Stores unique string keys pointing to their texture atlases.
    static std::unordered_map<std::string, TextureAtlas> s_atlasMap;
};