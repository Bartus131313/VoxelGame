#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

/// Represents type of shader compilation used for error check.
enum ShaderCompilationType {
    Vertex, Fragment, Program
};

/// Encapsulates an OpenGL shader program compiled from vertex and fragment GLSL files.
class Shader {
public:
    Shader() = default;
    ~Shader();

    // Prevent copying to avoid duplicate GPU program deletion issues
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    /// Loads, compiles, and links a vertex and fragment shader from disk.
    /// @param vertexPath Path to the GLSL vertex shader file.
    /// @param fragmentPath Path to the GLSL fragment shader file.
    /// @return True if compilation and linking succeed, false otherwise.
    bool loadFromFile(const std::string& vertexPath, const std::string& fragmentPath);

    /// Activates this shader program for rendering passes.
    void use() const;

    /// Gets the underlying OpenGL program handle.
    /// @return OpenGL program handle.
    [[nodiscard]] GLuint getID() const { return m_programID; }

    /// Checks if shader was loaded and can be used.
    /// @return @c true if shader is valid, @c false if not.
    [[nodiscard]] bool isValid() const { return m_programID != 0; }

    /// Sets bool value to Shader's uniform.
    /// @param name Name of uniform in shader.
    /// @param value Bool value which will be set.
    void setBool(const std::string& name, bool value) const;

    /// Sets int value to Shader's uniform.
    /// @param name Name of uniform in shader.
    /// @param value Int value which will be set.
    void setInt(const std::string& name, int value) const;

    /// Sets float to Shader's uniform.
    /// @param name Name of uniform in shader.
    /// @param value Float value which will be set.
    void setFloat(const std::string& name, float value) const;

    /// Sets vec2 value to Shader's uniform.
    /// @param name Name of uniform in shader.
    /// @param value Vec2 value which will be set.
    void setVec2(const std::string& name, const glm::vec2& value) const;

    /// Sets vec3 value to Shader's uniform.
    /// @param name Name of uniform in shader.
    /// @param value vec3 value which will be set.
    void setVec3(const std::string& name, const glm::vec3& value) const;

    /// Sets vec4 value to Shader's uniform.
    /// @param name Name of uniform in shader.
    /// @param value Vec4 value which will be set.
    void setVec4(const std::string& name, const glm::vec4& value) const;

    /// Sets mat4 value to Shader's uniform.
    /// @param name Name of uniform in shader.
    /// @param mat Mat4 value which will be set.
    void setMat4(const std::string& name, const glm::mat4& mat) const;
private:
    GLuint m_programID = 0; ///< OpenGL shader program ID.

    /// Helper utility to check for compilation and linking errors.
    /// @param shader Shader program ID.
    /// @param type Type of shader compilation (@c ShaderCompilationType).
    /// @return @c true if there was no error, @c false if there was an error.
    static bool checkCompileErrors(GLuint shader, ShaderCompilationType type);
};