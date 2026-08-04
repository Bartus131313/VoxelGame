#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

/** @brief Represents type of shader compilation used for error check. */
enum ShaderCompilationType {
    Vertex, Fragment, Geometry, Program, Compute
};

/** @brief Encapsulates an OpenGL shader program compiled from vertex and fragment GLSL files. */
class Shader {
public:
    Shader() = default;
    ~Shader();

    // Prevent copying to avoid duplicate GPU program deletion issues
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // Enable moving for efficient storage inside standard containers
    Shader(Shader&&) noexcept = default;
    Shader& operator=(Shader&&) noexcept = default;

    /**
     * @brief Loads, compiles, and links a shader program from disk.
     *
     * @param vertexPath Path to the GLSL vertex shader file.
     * @param fragmentPath Path to the GLSL fragment shader file.
     * @param geometryPath Optional path to the GLSL geometry shader file.
     * @return @c True if compilation and linking succeed, @c false otherwise.
     */
    bool loadFromFile(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "");

    /** @brief Activates this shader program for rendering passes. */
    void use() const;

    /**
     * @brief Gets the underlying OpenGL program handle.
     *
     * @return OpenGL program handle.
     */
    [[nodiscard]] GLuint getID() const { return m_programID; }

    /**
     * @brief Checks if shader was loaded and can be used.
     *
     * @return @c true if shader is valid, @c false if not.
     */
    [[nodiscard]] bool isValid() const { return m_programID != 0; }

    /**
     * @brief Sets bool value to Shader's uniform.
     *
     * @param name Name of uniform in shader.
     * @param value Bool value which will be set.
     */
    void setBool(const std::string& name, bool value) const;

    /**
     * @brief Sets int value to Shader's uniform.
     *
     * @param name Name of uniform in shader.
     * @param value Int value which will be set.
     */
    void setInt(const std::string& name, int value) const;

    /**
     * @brief Sets float value to Shader's uniform.
     *
     * @param name Name of uniform in shader.
     * @param value Float value which will be set.
     */
    void setFloat(const std::string& name, float value) const;

    /**
     * @brief Sets vec2 value to Shader's uniform.
     *
     * @param name Name of uniform in shader.
     * @param value Vec2 value which will be set.
     */
    void setVec2(const std::string& name, const glm::vec2& value) const;

    /**
     * @brief Sets vec3 value to Shader's uniform.
     *
     * @param name Name of uniform in shader.
     * @param value Vec3 value which will be set.
     */
    void setVec3(const std::string& name, const glm::vec3& value) const;

    /**
     * @brief Sets vec4 value to Shader's uniform.
     *
     * @param name Name of uniform in shader.
     * @param value vec4 value which will be set.
     */
    void setVec4(const std::string& name, const glm::vec4& value) const;

    /**
     * @brief Sets mat4 value to Shader's uniform.
     *
     * @param name Name of uniform in shader.
     * @param mat Mat4 value which will be set.
     */
    void setMat4(const std::string& name, const glm::mat4& mat) const;

    /**
     * @brief Binds a texture handle to an OpenGL texture slot and updates the sampler uniform in the shader.
     *
     * @param name Name of sampler uniform in shader (e.g., "u_Texture").
     * @param textureID OpenGL texture handle to bind.
     * @param slot Texture unit index (0 for GL_TEXTURE0, 1 for GL_TEXTURE1, etc.).
     * @param target Texture target type (defaults to GL_TEXTURE_2D, can also be GL_TEXTURE_CUBE_MAP, etc.).
     */
    void setTexture(const std::string& name, GLuint textureID, unsigned int slot = 0, GLenum target = GL_TEXTURE_2D) const;

private:
    GLuint m_programID = 0; ///< OpenGL shader program ID.

    /**
     * @brief Helper utility to check for compilation and linking errors.
     *
     * @param shader Shader program ID.
     * @param type Type of shader compilation (@c ShaderCompilationType).
     *
     * @return @c true if there was no error, @c false if there was an error.
     */
    static bool checkCompileErrors(GLuint shader, ShaderCompilationType type);
};