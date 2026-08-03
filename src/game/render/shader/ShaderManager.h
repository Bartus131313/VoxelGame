#pragma once

#include <memory>

#include "Shader.h"

/// Handles shader loading, unloading and caching.
class ShaderManager {
public:
    static constexpr auto SHADERS_PATH = "assets/shaders/";     ///< Default base directory for shader assets.

    /// Loads a new shader into memory or returns the cached version if it already exists.
    /// @param shaderFileName File name of the font relative to @c SHADERS_PATH folder <b>WITHOUT EXTENSION</b>.
    /// @param useGeometry Should the shader be compiled with geometry shader.
    /// @return Shared pointer to shader, or nullptr if loading failed.
    static std::shared_ptr<Shader> loadShader(const std::string& shaderFileName, bool useGeometry = false);

    /// Clears the shader cache, automatically invoking RAII cleanup on all loaded shader resources.
    static void cleanup();

private:
    /// Internal cache storing unique pointers to loaded shaders mapped by a unique identifier key.
    static std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;
};