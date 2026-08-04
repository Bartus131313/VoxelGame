#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

#include "glad/glad.h"
#include "stb_truetype.h"
#include "../../resource/ResourceLocation.h"

/** @brief FontKey struct is used to be as a key pointing to cached FontData. */
struct FontKey {
    ResourceLocation location;      ///< Resource Location pointing to the font file.
    int fontSize{16};               ///< Size of the font.

    // Required for unordered_map to compare keys
    bool operator==(const FontKey& other) const {
        return location == other.location && fontSize == other.fontSize;
    }
};

template <>
struct std::hash<FontKey> {
    std::size_t operator()(const FontKey& key) const noexcept {
        const std::size_t h1 = key.location.getHashCode();
        const std::size_t h2 = std::hash<int>{}(key.fontSize);

        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};

/** @brief Represents data of a loaded font, featuring automated VRAM cleanup via RAII. */
struct FontData {
    GLuint textureID = 0;                   ///< ID of OpenGL texture with all font chars.
    GLuint vao = 0;                         ///< Vertex array object ID.
    GLuint vbo = 0;                         ///< Vertex buffer object ID.
    float pixelAscent = 0.0f;               ///< Pixel ascent used for top-left anchoring.
    std::vector<stbtt_packedchar> charData; ///< Packed character metrics.

    /** @brief  Automatically frees GPU textures, buffers, and vertex arrays when destroyed. */
    ~FontData() {
        if (textureID) glDeleteTextures(1, &textureID);
        if (vbo) glDeleteBuffers(1, &vbo);
        if (vao) glDeleteVertexArrays(1, &vao);
    }

    // Prevent copying to avoid double-freeing GPU handles across memory boundaries
    FontData(const FontData&) = delete;
    FontData& operator=(const FontData&) = delete;

    /** @brief  Default constructor initializing empty font metadata. */
    FontData() = default;

    // Enable moving for efficient storage inside standard containers
    FontData(FontData&&) noexcept = default;
    FontData& operator=(FontData&&) noexcept = default;
};

/** @brief Handles font loading, unloading and caching. */
class FontManager {
public:
    static constexpr int  FONT_ATLAS_WIDTH  = 512; ///< Width of the generated font texture atlas in pixels.
    static constexpr int  FONT_ATLAS_HEIGHT = 512; ///< Height of the generated font texture atlas in pixels.

    /**
     * @brief Loads a new font into memory or returns the cached version if it already exists.
     *
     * @param fontLocation Resource Location pointing to the font file.
     * @param fontSize Size of the font in pixels.
     * 
     * @return Shared pointer to FontData, or nullptr if loading failed.
     */
    static std::shared_ptr<FontData> loadFont(const ResourceLocation& fontLocation, int fontSize);

    /** @brief Clears the font cache, automatically invoking RAII cleanup on all loaded font resources. */
    static void cleanup();

private:
    /** @brief Internal cache storing unique pointers to loaded font data mapped by a unique identifier key. */
    static std::unordered_map<FontKey, std::shared_ptr<FontData>> m_fonts;
};