#define STB_TRUETYPE_IMPLEMENTATION
#include "FontManager.h"

#include <fstream>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#include "../Vertex.h"
#include "../../../core/Logger.h"

std::unordered_map<FontKey, std::shared_ptr<FontData>> FontManager::m_fonts;

std::shared_ptr<FontData> FontManager::loadFont(const ResourceLocation& fontLocation, const int fontSize) {
    // Create the lookup key
    const FontKey cacheKey{fontLocation, fontSize};

    // Check if it already exists in the cache
    if (m_fonts.contains(cacheKey)) return m_fonts[cacheKey];

    // Resolve the path and load the font if not found
    std::string fullPath = fontLocation.resolveAssetPath();

    // Read TTF file into memory
    std::ifstream file(fullPath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open font file: {}", fullPath);
        return nullptr;
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<unsigned char> ttfBuffer(size);
    if (!file.read(reinterpret_cast<char*>(ttfBuffer.data()), size)) {
        LOG_ERROR("Failed to read font file data: {}", fullPath);
        return nullptr;
    }

    // Allocate heap-backed FontData wrapped in a unique_ptr
    auto fontData = std::make_shared<FontData>();
    fontData->charData.resize(96); // ASCII range 32 to 127

    // Allocate temporary bitmap for packing
    std::vector<unsigned char> pixels(FONT_ATLAS_WIDTH * FONT_ATLAS_HEIGHT);

    stbtt_fontinfo font;
    if (!stbtt_InitFont(&font, ttfBuffer.data(), 0)) {
        LOG_ERROR("Failed to initialize stb_truetype font info: {}", fullPath);
        return nullptr;
    }

    // Calculate pixel ascent for top-left anchoring
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);
    float scaleFactor = stbtt_ScaleForPixelHeight(&font, static_cast<float>(fontSize));
    fontData->pixelAscent = static_cast<float>(ascent) * scaleFactor;

    stbtt_pack_context pc;
    if (!stbtt_PackBegin(&pc, pixels.data(), FONT_ATLAS_WIDTH, FONT_ATLAS_HEIGHT, 0, 1, nullptr)) {
        LOG_ERROR("Failed to initialize stb font packer.");
        return nullptr;
    }

    stbtt_PackFontRange(&pc, ttfBuffer.data(), 0, static_cast<float>(fontSize), 32, 95, fontData->charData.data());
    stbtt_PackEnd(&pc);

    // Create OpenGL texture
    glGenTextures(1, &fontData->textureID);
    glBindTexture(GL_TEXTURE_2D, fontData->textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, FONT_ATLAS_WIDTH, FONT_ATLAS_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, pixels.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Setup persistent VAO & VBO once
    glGenVertexArrays(1, &fontData->vao);
    glGenBuffers(1, &fontData->vbo);

    glBindVertexArray(fontData->vao);
    glBindBuffer(GL_ARRAY_BUFFER, fontData->vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    // Position attribute (vec2)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), reinterpret_cast<void*>(offsetof(Vertex2D, x)));
    glEnableVertexAttribArray(0);

    // Texture Coordinate attribute (vec2)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), reinterpret_cast<void*>(offsetof(Vertex2D, u)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    LOG_DEBUG("Loaded and packed font {} ({} px)", fullPath, fontSize);

    // Cache and return
    m_fonts[cacheKey] = fontData;
    return fontData;
}

void FontManager::cleanup() {
    m_fonts.clear();
    LOG_INFO("Cleaned up all cached fonts.");
}