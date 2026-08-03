#include "ShaderManager.h"

#include <iostream>

// Initialize the static cache map
std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderManager::m_shaders;

std::shared_ptr<Shader> ShaderManager::loadShader(const std::string& shaderFileName, const bool useGeometry) {
    // Create a safe, unique cache key (e.g., "ui_shader_std" or "terrain_shader_geo")
    const std::string cacheKey = shaderFileName + (useGeometry ? "_geo" : "_std");

    // Check if the shader already exists in the cache
    if (const auto it = m_shaders.find(cacheKey); it != m_shaders.end()) {
        return it->second;
    }

    // Build full file paths
    const std::string vertPath = std::string(SHADERS_PATH) + shaderFileName + ".vsh";
    const std::string fragPath = std::string(SHADERS_PATH) + shaderFileName + ".fsh";
    const std::string geoPath  = useGeometry ? (std::string(SHADERS_PATH) + shaderFileName + ".gsh") : "";

    // Allocate the shader and delegate the heavy lifting to it
    auto shader = std::make_shared<Shader>();

    // We pass the geoPath as well. If it's empty, the Shader class will ignore it.
    if (!shader->loadFromFile(vertPath, fragPath, geoPath)) {
        std::cerr << "[ShaderManager] Failed to load shader asset: " << shaderFileName << "\n";
        return nullptr;
    }

    // Cache and return
    m_shaders[cacheKey] = shader;
    return shader;
}

void ShaderManager::cleanup() {
    m_shaders.clear();
    std::cout << "[ShaderManager] Cleaned up all cached shaders.\n";
}