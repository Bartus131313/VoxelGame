#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

#include "glad/glad.h"
#include "stb_truetype.h"

/// Represents data of a loaded font, featuring automated VRAM cleanup via RAII.
struct FontData {
    GLuint textureID = 0;                   ///< ID of OpenGL texture with all font chars.
    GLuint vao = 0;                         ///< Vertex array object ID.
    GLuint vbo = 0;                         ///< Vertex buffer object ID.
    float pixelAscent = 0.0f;               ///< Pixel ascent used for top-left anchoring.
    std::vector<stbtt_packedchar> charData; ///< Packed character metrics.

    /// RAutomatically frees GPU textures, buffers, and vertex arrays when destroyed.
    ~FontData() {
        if (textureID) glDeleteTextures(1, &textureID);
        if (vbo) glDeleteBuffers(1, &vbo);
        if (vao) glDeleteVertexArrays(1, &vao);
    }

    // Prevent copying to avoid double-freeing GPU handles across memory boundaries
    FontData(const FontData&) = delete;
    FontData& operator=(const FontData&) = delete;

    /// @brief Default constructor initializing empty font metadata.
    FontData() = default;

    // Enable moving for efficient storage inside standard containers
    FontData(FontData&&) noexcept = default;
    FontData& operator=(FontData&&) noexcept = default;
};

/// Handles font loading, unloading and caching.
class FontManager {
public:
    static constexpr auto FONTS_PATH        = "assets/fonts/"; ///< Default base directory for font assets.
    static constexpr int  FONT_ATLAS_WIDTH  = 512; ///< Width of the generated font texture atlas in pixels.
    static constexpr int  FONT_ATLAS_HEIGHT = 512; ///< Height of the generated font texture atlas in pixels.

    /// Loads a new font into memory or returns the cached version if it already exists.
    /// @param fontFileName File name of the font relative to @c FONTS_PATH folder.
    /// @param fontSize Size of the font in pixels.
    /// @return Pointer to FontData, or nullptr if loading failed.
    static FontData* loadFont(const std::string& fontFileName, int fontSize);

    /// Clears the font cache, automatically invoking RAII cleanup on all loaded font resources.
    static void cleanup();

private:
    /// Internal cache storing unique pointers to loaded font data mapped by a unique identifier key.
    static std::unordered_map<std::string, std::unique_ptr<FontData>> m_fonts;
};