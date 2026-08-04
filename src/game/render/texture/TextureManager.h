#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "glad/glad.h"

/** @brief Represents data of a loaded 2D texture, featuring automated VRAM cleanup via RAII. */
struct TextureData {
    GLuint id = 0;      ///< ID of the OpenGL texture object.
    int width = 0;      ///< Width of the texture in pixels.
    int height = 0;     ///< Height of the texture in pixels.
    int channels = 0;   ///< Number of color channels (e.g., 3 for RGB, 4 for RGBA).

    /** @brief Automatically frees GPU texture resource when destroyed. */
    ~TextureData() {
        if (id) glDeleteTextures(1, &id);
    }

    // Prevent copying to avoid double-freeing GPU handles across memory boundaries
    TextureData(const TextureData&) = delete;
    TextureData& operator=(const TextureData&) = delete;

    /** @brief Default constructor initializing empty texture metadata. */
    TextureData() = default;

    // Enable moving for efficient storage inside standard containers
    TextureData(TextureData&&) noexcept = default;
    TextureData& operator=(TextureData&&) noexcept = default;

    /**
     * @brief Checks if texture was successfully loaded and can be used.
     *
     * @return @c True if texture is valid, @c false if not.
     */
    [[nodiscard]] bool isValid() const { return id != 0; }
};

/** @brief Handles 2D texture loading, unloading and caching. */
class TextureManager {
public:
    static constexpr auto TEXTURES_PATH = "assets/textures/"; ///< Default base directory for texture assets.

    /**
     * @brief Loads a new texture into memory or returns the cached version if it already exists.
     *
     * @param textureFileName File name of the texture relative to @c TEXTURES_PATH folder.
     * @param flipVertically Whether to flip the image vertically on load (defaults to true for OpenGL UV space).
     * 
     * @return Shared pointer to TextureData, or nullptr if loading failed.
     */
    static std::shared_ptr<TextureData> loadTexture(const std::string& textureFileName, bool flipVertically = true);

    /** @brief Clears the texture cache, automatically invoking RAII cleanup on all loaded texture resources. */
    static void cleanup();

private:
    /** @brief Internal cache storing shared pointers to loaded texture data mapped by a unique identifier key. */
    static std::unordered_map<std::string, std::shared_ptr<TextureData>> m_textures;
};