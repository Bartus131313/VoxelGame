#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "../../resource/ResourceLocation.h"
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
     * @return @c true if texture is valid, @c false if not.
     */
    [[nodiscard]] bool isValid() const { return id != 0; }
};

/** @brief TextureKey struct is used to be as a key pointing to cached TextureData. */
struct TextureKey {
    ResourceLocation location;
    bool flipVertically{true};

    bool operator==(const TextureKey& other) const {
        return location == other.location && flipVertically == other.flipVertically;
    }
};

template <>
struct std::hash<TextureKey> {
    std::size_t operator()(const TextureKey& key) const noexcept {
        const std::size_t h1 = key.location.getHashCode();
        const std::size_t h2 = std::hash<bool>{}(key.flipVertically);

        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};

/** @brief Handles 2D texture loading, unloading and caching. */
class TextureManager {
public:
    /**
     * @brief Loads a new texture into memory or returns the cached version if it already exists.
     *
     * @param textureLocation Resource Location pointing to the texture file.
     * @param flipVertically Whether to flip the image vertically on load (defaults to true for OpenGL UV space).
     * 
     * @return Shared pointer to TextureData, or nullptr if loading failed.
     */
    static std::shared_ptr<TextureData> loadTexture(const ResourceLocation &textureLocation, bool flipVertically = true);

    /** @brief Clears the texture cache, automatically invoking RAII cleanup on all loaded texture resources. */
    static void cleanup();

private:
    /** @brief Internal cache storing shared pointers to loaded texture data mapped by a unique identifier key. */
    static std::unordered_map<TextureKey, std::shared_ptr<TextureData>> m_textures;
};