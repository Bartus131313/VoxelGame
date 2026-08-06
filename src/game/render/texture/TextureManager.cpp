#define STB_IMAGE_IMPLEMENTATION
#include "TextureManager.h"
#include "stb_image.h"

#include <iostream>

#include "../RenderSystem.h"
#include "../../../core/Logger.h"

std::unordered_map<TextureKey, std::shared_ptr<TextureData>> TextureManager::m_textures;

std::shared_ptr<TextureData> TextureManager::loadTexture(const ResourceLocation& textureLocation, const bool flipVertically) {
    // Create the stack-allocated lookup key
    const TextureKey cacheKey{textureLocation, flipVertically};

    // Check if the texture is already loaded with these exact settings
    if (m_textures.contains(cacheKey)) return m_textures[cacheKey];

    // Resolve the path
    std::string fullPath = textureLocation.resolveAssetPath();

    // Configure stb_image vertical flip for OpenGL coordinate origin
    stbi_set_flip_vertically_on_load(flipVertically ? 1 : 0);

    // Read image file into CPU memory
    int width = 0, height = 0, channels = 0;
    unsigned char* pixels = stbi_load(fullPath.c_str(), &width, &height, &channels, 0);

    if (!pixels) {
        LOG_ERROR("Failed to load image file: {}", fullPath);
        return nullptr;
    }

    // Determine target formats based on channels
    GLenum internalFormat = GL_RGB8;
    GLenum dataFormat = GL_RGB;

    if (channels == 1) {
        internalFormat = GL_R8;
        dataFormat = GL_RED;
    } else if (channels == 3) {
        internalFormat = GL_RGB8;
        dataFormat = GL_RGB;
    } else if (channels == 4) {
        internalFormat = GL_RGBA8;
        dataFormat = GL_RGBA;
    }

    // Allocate heap-backed TextureData
    auto textureData = std::make_shared<TextureData>();
    textureData->width = width;
    textureData->height = height;
    textureData->channels = channels;

    // Create and configure OpenGL texture
    glGenTextures(1, &textureData->id);
    RenderSystem::bindTexture(TextureType::Texture2D, textureData->id);

    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(internalFormat), width, height, 0, dataFormat, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Pixelated/Nearest filtering (ideal for pixel art / voxel textures)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    RenderSystem::bindTexture(TextureType::Texture2D, 0);

    // Free CPU image data buffer
    stbi_image_free(pixels);

    LOG_DEBUG("Loaded texture {} ({}x{}, {} channels)", fullPath, width, height, channels);

    // Cache and return
    m_textures[cacheKey] = textureData;
    return textureData;
}

void TextureManager::cleanup() {
    m_textures.clear();
    LOG_INFO("Cleaned up all cached textures.");
}