#include "TextureAtlasManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <ranges>

#include "../RenderSystem.h"
#include "../../../core/Logger.h"

namespace fs = std::filesystem;

const ResourceLocation TextureAtlasManager::texturesLocation{"textures"};
std::unordered_map<std::string, TextureAtlas> TextureAtlasManager::s_atlasMap{};
bool TextureAtlasManager::forceRebuildCache = false;

namespace {

/** @brief Helper structure to hold texture data before packing. */
struct RawTextureAsset {
    std::string key;
    bool isAnimated;
    int width, height;
    std::vector<uint8_t> staticPixels;
    std::vector<std::vector<uint8_t>> animFrames;
};

constexpr uint64_t FNV_OFFSET = 14695981039346656037ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

void hashCombine(uint64_t& hash, const void* data, const size_t size) {
    const auto* bytes = static_cast<const uint8_t*>(data);
    for (size_t i = 0; i < size; ++i) {
        hash ^= bytes[i];
        hash *= FNV_PRIME;
    }
}

void hashString(uint64_t& hash, const std::string& s) {
    hashCombine(hash, s.data(), s.size());
}

/**
 * @brief Computes a deterministic hash over every file in a directory tree
 * (path + size + last-write-time), so any texture add/remove/edit invalidates the cache.
 */
uint64_t hashDirectory(const fs::path& dir) {
    std::vector<fs::path> files;
    for (const auto& entry : fs::recursive_directory_iterator(dir)) {
        if (entry.is_regular_file()) files.push_back(entry.path());
    }
    // Sort for determinism - directory iteration order is not guaranteed.
    std::ranges::sort(files);

    uint64_t hash = FNV_OFFSET;
    for (const auto& path : files) {
        std::string rel = fs::relative(path, dir).string();
        hashString(hash, rel);

        auto size = fs::file_size(path);
        hashCombine(hash, &size, sizeof(size));

        auto writeTime = fs::last_write_time(path).time_since_epoch().count();
        hashCombine(hash, &writeTime, sizeof(writeTime));
    }
    return hash;
}

/**
 * @brief Single source of truth for shelf-packing. Used both to *simulate* fitting
 * (to find the smallest power-of-2 atlas size) and to compute *final* placements,
 * so the two passes can never drift apart.
 *
 * @return true if every asset fit within [atlasWidth x atlasHeight].
 */
bool packRects(const std::vector<RawTextureAsset>& assets, const int atlasWidth, const int atlasHeight,
               const int padding, std::vector<PackedRect>* outPlacements) {
    int cursorX = 0;
    int cursorY = 0;
    int maxHeightInRow = 0;

    if (outPlacements) outPlacements->clear();

    for (const auto& asset : assets) {
        if (cursorX + asset.width + padding > atlasWidth) {
            cursorX = 0;
            cursorY += maxHeightInRow + padding;
            maxHeightInRow = 0;
        }

        if (cursorY + asset.height + padding > atlasHeight) {
            return false;
        }

        if (outPlacements) {
            outPlacements->push_back({asset.key, cursorX, cursorY, asset.width, asset.height, asset.isAnimated});
        }

        maxHeightInRow = std::max(maxHeightInRow, asset.height);
        cursorX += asset.width + padding;
    }
    return true;
}

/** @brief Finds the smallest power-of-2 square atlas size that fits every asset. */
bool findAtlasSize(const std::vector<RawTextureAsset>& assets, const int padding, int* outWidth, int* outHeight) {
    int size = 64;
    while (size <= 8192) {
        if (packRects(assets, size, size, padding, nullptr)) {
            *outWidth = size;
            *outHeight = size;
            return true;
        }
        size *= 2;
    }
    return false;
}

// --- Binary cache format helpers ---------------------------------------------------

void writeString(std::ofstream& f, const std::string& s) {
    const auto len = static_cast<uint32_t>(s.size());
    f.write(reinterpret_cast<const char*>(&len), sizeof(len));
    f.write(s.data(), len);
}

std::string readString(std::ifstream& f) {
    uint32_t len = 0;
    f.read(reinterpret_cast<char*>(&len), sizeof(len));
    std::string s(len, '\0');
    f.read(s.data(), len);
    return s;
}

} // namespace

std::string TextureAtlasManager::getCacheDir() {
    std::string dir = "cache/textures";
    if (!fs::exists(dir)) {
        fs::create_directories(dir);
    }
    return dir;
}

/**
 * @brief Writes a fully-packed atlas (pixels + uv/anim metadata) to a single binary cache file.
 */
static void writeAtlasCache(const fs::path& cachePath, uint64_t sourceHash, const TextureAtlas& atlas,
                             const std::vector<uint8_t>& pixels) {
    std::ofstream f(cachePath, std::ios::binary);
    if (!f) return;

    f.write(reinterpret_cast<const char*>(&sourceHash), sizeof(sourceHash));
    f.write(reinterpret_cast<const char*>(&atlas.width), sizeof(atlas.width));
    f.write(reinterpret_cast<const char*>(&atlas.height), sizeof(atlas.height));
    f.write(reinterpret_cast<const char*>(pixels.data()), static_cast<std::streamsize>(pixels.size()));

    const auto uvCount = static_cast<uint32_t>(atlas.uvMap.size());
    f.write(reinterpret_cast<const char*>(&uvCount), sizeof(uvCount));
    for (const auto& [key, uv] : atlas.uvMap) {
        writeString(f, key);
        f.write(reinterpret_cast<const char*>(&uv), sizeof(uv));
    }

    const auto animCount = static_cast<uint32_t>(atlas.animMap.size());
    f.write(reinterpret_cast<const char*>(&animCount), sizeof(animCount));
    for (const auto& [key, anim] : atlas.animMap) {
        writeString(f, key);
        f.write(reinterpret_cast<const char*>(&anim.atlasX), sizeof(anim.atlasX));
        f.write(reinterpret_cast<const char*>(&anim.atlasY), sizeof(anim.atlasY));
        f.write(reinterpret_cast<const char*>(&anim.tileWidth), sizeof(anim.tileWidth));
        f.write(reinterpret_cast<const char*>(&anim.tileHeight), sizeof(anim.tileHeight));
        f.write(reinterpret_cast<const char*>(&anim.frameDuration), sizeof(anim.frameDuration));

        const auto frameCount = static_cast<uint32_t>(anim.frames.size());
        f.write(reinterpret_cast<const char*>(&frameCount), sizeof(frameCount));
        for (const auto& frame : anim.frames) {
            const auto frameSize = static_cast<uint32_t>(frame.size());
            f.write(reinterpret_cast<const char*>(&frameSize), sizeof(frameSize));
            f.write(reinterpret_cast<const char*>(frame.data()), frameSize);
        }
    }
}

/** @brief Loads a cached atlas back. Returns false (and leaves state untouched) if the file is missing/corrupt. */
static bool readAtlasCache(const fs::path& cachePath, uint64_t expectedHash, TextureAtlas* outAtlas,
                            std::vector<uint8_t>* outPixels) {
    std::ifstream f(cachePath, std::ios::binary);
    if (!f) return false;

    uint64_t storedHash = 0;
    f.read(reinterpret_cast<char*>(&storedHash), sizeof(storedHash));
    if (!f || storedHash != expectedHash) return false;

    f.read(reinterpret_cast<char*>(&outAtlas->width), sizeof(outAtlas->width));
    f.read(reinterpret_cast<char*>(&outAtlas->height), sizeof(outAtlas->height));
    if (!f || outAtlas->width <= 0 || outAtlas->height <= 0) return false;

    outPixels->resize(static_cast<size_t>(outAtlas->width) * outAtlas->height * 4);
    f.read(reinterpret_cast<char*>(outPixels->data()), static_cast<std::streamsize>(outPixels->size()));
    if (!f) return false;

    uint32_t uvCount = 0;
    f.read(reinterpret_cast<char*>(&uvCount), sizeof(uvCount));
    for (uint32_t i = 0; i < uvCount; ++i) {
        std::string key = readString(f);
        UVBox uv{};
        f.read(reinterpret_cast<char*>(&uv), sizeof(uv));
        outAtlas->uvMap[key] = uv;
    }

    uint32_t animCount = 0;
    f.read(reinterpret_cast<char*>(&animCount), sizeof(animCount));
    for (uint32_t i = 0; i < animCount; ++i) {
        std::string key = readString(f);
        AnimatedTexture anim{};
        f.read(reinterpret_cast<char*>(&anim.atlasX), sizeof(anim.atlasX));
        f.read(reinterpret_cast<char*>(&anim.atlasY), sizeof(anim.atlasY));
        f.read(reinterpret_cast<char*>(&anim.tileWidth), sizeof(anim.tileWidth));
        f.read(reinterpret_cast<char*>(&anim.tileHeight), sizeof(anim.tileHeight));
        f.read(reinterpret_cast<char*>(&anim.frameDuration), sizeof(anim.frameDuration));

        uint32_t frameCount = 0;
        f.read(reinterpret_cast<char*>(&frameCount), sizeof(frameCount));
        anim.frames.reserve(frameCount);
        for (uint32_t fi = 0; fi < frameCount; ++fi) {
            uint32_t frameSize = 0;
            f.read(reinterpret_cast<char*>(&frameSize), sizeof(frameSize));
            std::vector<uint8_t> frame(frameSize);
            f.read(reinterpret_cast<char*>(frame.data()), frameSize);
            anim.frames.push_back(std::move(frame));
        }

        // Also rebuild the UV entry for the animated key (upload uses uvMap for lookup too).
        outAtlas->uvMap[key] = outAtlas->uvMap.count(key) ? outAtlas->uvMap[key]
            : UVBox{}; // will be overwritten below if present in cache
        outAtlas->animMap[key] = std::move(anim);
    }

    return f.good() || f.eof();
}

static GLuint uploadAtlasTexture(int width, int height, const std::vector<uint8_t>& pixels) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    RenderSystem::bindTexture(TextureType::Texture2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // Clamp instead of the GL default (repeat) - repeat can sample the opposite edge of a
    // neighbouring tile at atlas seams under linear filtering / mip sampling, which looks
    // like "wrong" pixels bleeding in near tile borders.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return textureID;
}

void TextureAtlasManager::init() {
    std::string baseTexturesPath = texturesLocation.resolveAssetPath();

    if (!fs::exists(baseTexturesPath) || !fs::is_directory(baseTexturesPath)) {
        LOG_ERROR("Base textures directory not found at: {}", baseTexturesPath);
        return;
    }

    std::string cacheDir = getCacheDir();

    for (const auto& atlasDirEntry : fs::directory_iterator(baseTexturesPath)) {
        if (!atlasDirEntry.is_directory()) continue;

        std::string atlasName = atlasDirEntry.path().filename().string();
        uint64_t sourceHash = hashDirectory(atlasDirEntry.path());
        fs::path cachePath = fs::path(cacheDir) / (atlasName + ".atlascache");

        // --- Try the cache first ---
        if (!forceRebuildCache) {
            TextureAtlas cached{};
            std::vector<uint8_t> pixels;
            if (readAtlasCache(cachePath, sourceHash, &cached, &pixels)) {
                cached.textureID = uploadAtlasTexture(cached.width, cached.height, pixels);
                s_atlasMap[atlasName] = std::move(cached);
                LOG_INFO("Loaded atlas '{}' from cache ({}x{})", atlasName, s_atlasMap[atlasName].width,
                         s_atlasMap[atlasName].height);
                continue;
            }
        }

        // --- Cache miss: decode + pack from source images ---
        std::vector<RawTextureAsset> rawAssets;

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
                asset.isAnimated = false;
                asset.width = imgWidth;
                asset.height = imgHeight;
                asset.staticPixels.assign(pixelData, pixelData + (imgWidth * imgHeight * 4));
            }

            stbi_image_free(pixelData);
            rawAssets.push_back(std::move(asset));
        }

        if (rawAssets.empty()) continue;

        // Sort by height descending - makes shelf rows tighter.
        std::ranges::sort(rawAssets, [](const RawTextureAsset& a, const RawTextureAsset& b) {
            return a.height > b.height;
        });

        constexpr int padding = 2;
        int atlasWidth = 0, atlasHeight = 0;

        if (!findAtlasSize(rawAssets, padding, &atlasWidth, &atlasHeight)) {
            LOG_ERROR("Atlas '{}' exceeded maximum dynamic size (8192x8192)! Too many assets.", atlasName);
            continue;
        }

        std::vector<PackedRect> placements;
        packRects(rawAssets, atlasWidth, atlasHeight, padding, &placements);

        std::vector<uint8_t> cpuPixels(static_cast<size_t>(atlasWidth) * atlasHeight * 4, 0);

        // Build a lookup from key -> raw asset for pixel access during blit.
        std::unordered_map<std::string, RawTextureAsset*> assetByKey;
        for (auto& asset : rawAssets) assetByKey[asset.key] = &asset;

        TextureAtlas atlas{};
        atlas.width = atlasWidth;
        atlas.height = atlasHeight;

        for (const auto& rect : placements) {
            RawTextureAsset* asset = assetByKey[rect.key];
            const uint8_t* srcPixels = rect.isAnimated ? asset->animFrames[0].data() : asset->staticPixels.data();

            // Blit into the CPU-side buffer (row by row) so the cache and the GPU upload
            // always contain exactly what was placed - no separate re-derivation.
            for (int y = 0; y < rect.height; ++y) {
                std::memcpy(
                    cpuPixels.data() + ((rect.y + y) * atlasWidth + rect.x) * 4,
                    srcPixels + (y * rect.width) * 4,
                    rect.width * 4
                );
            }

            atlas.uvMap[rect.key] = calculateUVs(rect.x, rect.y, rect.width, rect.height, atlasWidth, atlasHeight);

            if (rect.isAnimated) {
                AnimatedTexture anim{};
                anim.atlasX = rect.x;
                anim.atlasY = rect.y;
                anim.tileWidth = rect.width;
                anim.tileHeight = rect.height;
                anim.frames = std::move(asset->animFrames);
                anim.frameDuration = 3;
                atlas.animMap[rect.key] = std::move(anim);
            }
        }

        atlas.textureID = uploadAtlasTexture(atlasWidth, atlasHeight, cpuPixels);

        writeAtlasCache(cachePath, sourceHash, atlas, cpuPixels);

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
    std::vector<uint8_t> pixels(static_cast<size_t>(atlas.width) * atlas.height * 4);

    RenderSystem::bindTexture(TextureType::Texture2D, atlas.textureID);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    const int success = stbi_write_png(outputPath.c_str(), atlas.width, atlas.height, 4, pixels.data(), atlas.width * 4);

    if (success) {
        LOG_INFO("Successfully dumped atlas '{}' to disk at: '{}'", atlasName, outputPath);
    } else {
        LOG_ERROR("Failed to write atlas image file to: '{}'", outputPath);
    }
}

void TextureAtlasManager::saveAllAtlases(const std::string& outputDir) {
    if (!fs::exists(outputDir)) {
        fs::create_directories(outputDir);
    }

    for (const auto& atlasName : s_atlasMap | std::views::keys) {
        fs::path outputPath = fs::path(outputDir) / (atlasName + ".png");
        saveAtlasFile(atlasName, outputPath.string());
    }
}

void TextureAtlasManager::update(const float deltaTime) {
    for (auto& atlas : s_atlasMap | std::views::values) {
        for (auto& anim : atlas.animMap | std::views::values) {
            const float durationInSeconds = static_cast<float>(anim.frameDuration) * (1.0f / 20.0f);

            anim.timeAccumulator += deltaTime;

            if (anim.timeAccumulator >= durationInSeconds) {
                anim.timeAccumulator = 0.0f;
                anim.currentFrame = (anim.currentFrame + 1) % anim.frames.size();

                RenderSystem::bindTexture(TextureType::Texture2D, atlas.textureID);
                glTexSubImage2D(
                    GL_TEXTURE_2D,
                    0,
                    anim.atlasX,
                    anim.atlasY,
                    anim.tileWidth,
                    anim.tileHeight,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    anim.frames[anim.currentFrame].data()
                );
            }
        }
    }
}

UVBox TextureAtlasManager::getUVBox(const std::string& atlasName, const std::string& textureName) {
    if (atlasName == "core" && !s_atlasMap.contains("core")) {
        return {0.0f, 0.0f, 1.0f, 1.0f};
    }

    const auto atlasIt = s_atlasMap.find(atlasName);
    if (atlasIt == s_atlasMap.end()) {
        return getUVBox("core", "missing");
    }

    const auto& atlas = atlasIt->second;

    const auto textureIt = atlas.uvMap.find(textureName);
    if (textureIt == atlas.uvMap.end()) {
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
    for (auto& atlas : s_atlasMap | std::views::values) {
        if (atlas.textureID != 0) {
            glDeleteTextures(1, &atlas.textureID);
        }
    }

    s_atlasMap.clear();
    LOG_INFO("Texture atlases cleaned up.");
}