#include "ShaderManager.h"

#include <iostream>

#include "../../../core/Logger.h"

// Initialize the static cache map
std::unordered_map<ShaderKey, std::shared_ptr<Shader>> ShaderManager::m_shaders;

std::shared_ptr<Shader> ShaderManager::loadShader(const ResourceLocation& shaderLocation, const bool useGeometry) {
    // Create the stack-allocated lookup key
    const ShaderKey cacheKey{shaderLocation, useGeometry};

    // Check if the shader is already compiled and cached
    if (m_shaders.contains(cacheKey)) return m_shaders[cacheKey];

    const std::string shaderFileName = shaderLocation.resolveAssetPath();

    // Build full file paths
    const std::string vertPath = shaderFileName + ".vsh";
    const std::string fragPath = shaderFileName + ".fsh";
    const std::string geoPath  = useGeometry ? (shaderFileName + ".gsh") : "";

    // Allocate the shader and delegate the heavy lifting to it
    auto shader = std::make_shared<Shader>();

    // We pass the geoPath as well. If it's empty, the Shader class will ignore it.
    if (!shader->loadFromFile(vertPath, fragPath, geoPath)) {
        LOG_ERROR("Failed to load shader asset {}", shaderFileName);
        return nullptr;
    }

    // Cache and return
    m_shaders[cacheKey] = shader;
    return shader;
}

void ShaderManager::cleanup() {
    m_shaders.clear();
    LOG_INFO("Cleaned up all cached shaders.");
}