#pragma once

#include <memory>

#include "Shader.h"
#include "../../resource/ResourceLocation.h"

/** @brief ShaderKey struct is used to be as a key pointing to cached Shader. */
struct ShaderKey {
    ResourceLocation location;      ///< Resource Location pointing to the shader file name.
    bool useGeometry{false};        ///< Should the geometry shader be used?

    bool operator==(const ShaderKey& other) const {
        return location == other.location && useGeometry == other.useGeometry;
    }
};

template <>
struct std::hash<ShaderKey> {
    std::size_t operator()(const ShaderKey& key) const noexcept {
        const std::size_t h1 = key.location.getHashCode();
        const std::size_t h2 = std::hash<bool>{}(key.useGeometry);

        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};

/** @brief Handles shader loading, unloading and caching. */
class ShaderManager {
public:
    /** @brief Loads a new shader into memory or returns the cached version if it already exists.
     *
     * @param shaderLocation Resource Location pointing to the shader file name without file extension.
     * @param useGeometry Should the shader be compiled with geometry shader.
     *
     * @return Shared pointer to shader, or nullptr if loading failed.
     */
    static std::shared_ptr<Shader> loadShader(const ResourceLocation& shaderLocation, bool useGeometry = false);

    /** @brief Clears the shader cache, automatically invoking RAII cleanup on all loaded shader resources. */
    static void cleanup();

private:
    /** @brief Internal cache storing unique pointers to loaded shaders mapped by a unique identifier key. */
    static std::unordered_map<ShaderKey, std::shared_ptr<Shader>> m_shaders;
};