#include "TextureAtlasManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <ranges>

#include "../RenderSystem.h"
#include "../../../core/Logger.h"

namespace fs = std::filesystem;

const ResourceLocation TextureAtlasManager::texturesLocation{"textures"};
std::unordered_map<std::string, TextureAtlas> TextureAtlasManager::s_atlasMap{};

/** @brief Helper structure to hold texture data before packing. */
struct RawTextureAsset {
    std::string key;
    bool isAnimated;
    int width, height;
    std::vector<uint8_t> staticPixels;
    std::vector<std::vector<uint8_t>> animFrames;
};

void TextureAtlasManager::init() {
    std::string baseTexturesPath = texturesLocation.resolveAssetPath();

    if (!fs::exists(baseTexturesPath) || !fs::is_directory(baseTexturesPath)) {
        LOG_ERROR("Base textures directory not found at: {}", baseTexturesPath);
        return;
    }

    // Iterate through top-level directories (Each directory = 1 Atlas)
    for (const auto& atlasDirEntry : fs::directory_iterator(baseTexturesPath)) {
        if (!atlasDirEntry.is_directory()) continue;

        std::string atlasName = atlasDirEntry.path().filename().string();

        std::vector<RawTextureAsset> rawAssets;

        // Recursively scan and load all textures into CPU memory
        for (const auto& fileEntry : fs::recursive_directory_iterator(atlasDirEntry.path())) {
            if (!fileEntry.is_regular_file()) continue;

            std::string ext = fileEntry.path().extension().string();
            if (ext != ".png" && ext != ".jpg") continue;

            std::string filePath = fileEntry.path().string();
            std::string textureKey = fs::relative(fileEntry.path(), atlasDirEntry.path()).replace_extension("").string();
            std::replace(textureKey.begin(), textureKey.end(), '\\', '/');

            int imgWidth = 0, imgHeight = 0, channels = 0;
            stbi_set_flip_vertically_on_load(false);
            unsigned char* pixelData = stbi_load(filePath.c_str(), &imgWidth, &imgHeight, &channels, 4);

            if (!pixelData) {
                LOG_ERROR("Failed to load texture file: {}", filePath);
                continue;
            }

            RawTextureAsset asset{};
            asset.key = textureKey;

            // Check if it's an animated vertical strip
            if (imgHeight > imgWidth && imgHeight % imgWidth == 0) {
                int frameSize = imgWidth;
                int frameCount = imgHeight / imgWidth;

                asset.isAnimated = true;
                asset.width = frameSize;
                asset.height = frameSize;

                for (int i = 0; i < frameCount; ++i) {
                    std::vector<uint8_t> framePixels(frameSize * frameSize * 4);
                    for (int y = 0; y < frameSize; ++y) {
                        int sourceY = (i * frameSize) + y;
                        std::memcpy(
                            framePixels.data() + (y * frameSize * 4),
                            pixelData + (sourceY * imgWidth * 4),
                            frameSize * 4
                        );
                    }
                    asset.animFrames.push_back(std::move(framePixels));
                }
            } else {
                // Static texture
                asset.isAnimated = false;
                asset.width = imgWidth;
                asset.height = imgHeight;
                asset.staticPixels.assign(pixelData, pixelData + (imgWidth * imgHeight * 4));
            }

            stbi_image_free(pixelData);
            rawAssets.push_back(std::move(asset));
        }

        if (rawAssets.empty()) continue;

        // Sort assets by height descending to make shelf-packing tighter and cleaner
        std::ranges::sort(rawAssets, [](const RawTextureAsset& a, const RawTextureAsset& b) {
            return a.height > b.height;
        });

        int padding = 2; // Gap to prevent color bleeding
        int atlasWidth = 64;
        int atlasHeight = 64;
        bool packedSuccessfully = false;

        // Dynamic Power-of-2 Sizing Loop
        while (!packedSuccessfully && atlasWidth <= 8192) {
            int cursorX = 0;
            int cursorY = 0;
            int maxHeightInRow = 0;
            bool failed = false;

            for (const auto& asset : rawAssets) {
                if (cursorX + asset.width + padding > atlasWidth) {
                    cursorX = 0;
                    cursorY += maxHeightInRow + padding;
                    maxHeightInRow = 0;
                }

                if (cursorY + asset.height + padding > atlasHeight) {
                    failed = true;
                    break;
                }

                maxHeightInRow = std::max(maxHeightInRow, asset.height);
                cursorX += asset.width + padding;
            }

            if (failed) {
                atlasWidth *= 2;
                atlasHeight *= 2;
            } else {
                packedSuccessfully = true;
            }
        }

        if (!packedSuccessfully) {
            LOG_ERROR("Atlas '{}' exceeded maximum dynamic size (8192x8192)! Too many assets.", atlasName);
            continue;
        }

        TextureAtlas atlas{};
        atlas.width = atlasWidth;
        atlas.height = atlasHeight;

        glGenTextures(1, &atlas.textureID);
        RenderSystem::bindTexture(TextureType::Texture2D, atlas.textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, atlas.width, atlas.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Final Packing & GPU Upload Phase
        int cursorX = 0;
        int cursorY = 0;
        int maxHeightInRow = 0;

        for (auto& asset : rawAssets) {
            if (cursorX + asset.width + padding > atlas.width) {
                cursorX = 0;
                cursorY += maxHeightInRow + padding;
                maxHeightInRow = 0;
            }

            int packedX = cursorX;
            int packedY = cursorY;

            maxHeightInRow = std::max(maxHeightInRow, asset.height);
            cursorX += asset.width + padding;

            if (asset.isAnimated) {
                glTexSubImage2D(GL_TEXTURE_2D, 0, packedX, packedY, asset.width, asset.height,
                                GL_RGBA, GL_UNSIGNED_BYTE, asset.animFrames[0].data());

                AnimatedTexture anim{};
                anim.atlasX = packedX;
                anim.atlasY = packedY;
                anim.tileWidth = asset.width;
                anim.tileHeight = asset.height;
                anim.frames = std::move(asset.animFrames);
                anim.frameDuration = 3;

                atlas.animMap[asset.key] = std::move(anim);
                atlas.uvMap[asset.key] = calculateUVs(packedX, packedY, asset.width, asset.height, atlas.width, atlas.height);
            } else {
                glTexSubImage2D(GL_TEXTURE_2D, 0, packedX, packedY, asset.width, asset.height,
                                GL_RGBA, GL_UNSIGNED_BYTE, asset.staticPixels.data());

                atlas.uvMap[asset.key] = calculateUVs(packedX, packedY, asset.width, asset.height, atlas.width, atlas.height);
            }
        }

        s_atlasMap[atlasName] = std::move(atlas);
        LOG_INFO("Packed and initialized atlas '{}' {}x{}", atlasName, atlasWidth, atlasHeight);
    }
}

void TextureAtlasManager::saveAtlasFile(const std::string& atlasName, const std::string& outputPath) {
    const auto it = s_atlasMap.find(atlasName);
    if (it == s_atlasMap.end()) {
        LOG_ERROR("Cannot save atlas '{}': Atlas not found.", atlasName);
        return;
    }

    const auto& atlas = it->second;
    std::vector<uint8_t> pixels(atlas.width * atlas.height * 4);

    // Bind the atlas texture and read its pixel data back from the GPU VRAM
    RenderSystem::bindTexture(TextureType::Texture2D, atlas.textureID);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    // Write the raw buffer to a PNG file using stb_image_write
    const int success = stbi_write_png(outputPath.c_str(), atlas.width, atlas.height, 4, pixels.data(), atlas.width * 4);

    if (success) {
        LOG_INFO("Successfully dumped atlas '{}' to disk at: '{}'", atlasName, outputPath);
    } else {
        LOG_ERROR("Failed to write atlas image file to: '{}'", outputPath);
    }
}

void TextureAtlasManager::update(const float deltaTime) {
    for (auto& atlas : s_atlasMap | std::views::values) {
        for (auto& anim : atlas.animMap | std::views::values) {
            // Convert ticks to seconds
            const float durationInSeconds = static_cast<float>(anim.frameDuration) * (1.0f / 20.0f);

            anim.timeAccumulator += deltaTime;

            if (anim.timeAccumulator >= durationInSeconds) {
                anim.timeAccumulator = 0.0f;
                anim.currentFrame = (anim.currentFrame + 1) % anim.frames.size();

                // Update atlas texture with new frame
                RenderSystem::bindTexture(TextureType::Texture2D, atlas.textureID);
                glTexSubImage2D(
                    GL_TEXTURE_2D,
                    0,                      // Mipmap level 0
                    anim.atlasX,            // X offset inside the giant atlas
                    anim.atlasY,            // Y offset inside the giant atlas
                    anim.tileWidth,         // Width of the sub-image
                    anim.tileHeight,        // Height of the sub-image
                    GL_RGBA,                // Pixel format
                    GL_UNSIGNED_BYTE,       // Data type
                    anim.frames[anim.currentFrame].data() // Pointer to the raw frame bytes
                );
            }
        }
    }
}

UVBox TextureAtlasManager::getUVBox(const std::string &atlasName, const std::string &textureName) {
    if (atlasName == "core" && !s_atlasMap.contains("core")) {
        return {0.0f, 0.0f, 1.0f, 1.0f};
    }

    // Find the target atlas
    const auto atlasIt = s_atlasMap.find(atlasName);
    if (atlasIt == s_atlasMap.end()) {
        // Fallback to a missing texture inside `core` atlas
        return getUVBox("core", "missing");
    }

    const auto& atlas = atlasIt->second;

    // Find the target texture inside the atlas
    const auto textureIt = atlas.uvMap.find(textureName);
    if (textureIt == atlas.uvMap.end()) {
        // Fallback to a missing texture inside `core` atlas
        return getUVBox("core", "missing");
    }

    return textureIt->second;
}

GLuint TextureAtlasManager::getAtlasTextureID(const std::string& atlasName) {
    const auto it = s_atlasMap.find(atlasName);
    if (it != s_atlasMap.end()) {
        return it->second.textureID;
    }
    return 0;
}

void TextureAtlasManager::cleanup() {
    // Properly delete GPU textures to prevent VRAM leaks
    for (auto& atlas : s_atlasMap | std::views::values) {
        if (atlas.textureID != 0) {
            glDeleteTextures(1, &atlas.textureID);
        }
    }

    s_atlasMap.clear();
    LOG_INFO("Texture atlases cleaned up.");
}
