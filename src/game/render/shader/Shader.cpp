#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

#include "../../../core/Logger.h"

Shader::~Shader() {
    if (m_programID != 0) glDeleteProgram(m_programID);
}

bool Shader::loadFromFile(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath) {
    std::string vertexCode, fragmentCode, geometryCode;
    std::ifstream vShaderFile, fShaderFile, gShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        // Only read geometry shader if a path was provided
        if (!geometryPath.empty()) {
            gShaderFile.open(geometryPath);
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            geometryCode = gShaderStream.str();
        }
    }
    catch (const std::ifstream::failure& e) {
        LOG_ERROR("Failed to read shader files from disk!");
        return false;
    }

    // Compile Vertex
    const char* vShaderCode = vertexCode.c_str();
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    if (!checkCompileErrors(vertex, Vertex)) return false;

    // Compile Fragment
    const char* fShaderCode = fragmentCode.c_str();
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    if (!checkCompileErrors(fragment, Fragment)) return false;

    // Compile Geometry (Optional)
    GLuint geometry = 0;
    if (!geometryPath.empty()) {
        const char* gShaderCode = geometryCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, nullptr);
        glCompileShader(geometry);
        if (!checkCompileErrors(geometry, Geometry)) return false;
    }

    // Link Shader Program
    m_programID = glCreateProgram();
    glAttachShader(m_programID, vertex);
    glAttachShader(m_programID, fragment);
    if (!geometryPath.empty()) {
        glAttachShader(m_programID, geometry);
    }
    glLinkProgram(m_programID);

    // Clean up
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (!geometryPath.empty()) {
        glDeleteShader(geometry);
    }

    if (!checkCompileErrors(m_programID, Program)) {
        m_programID = 0;
        return false;
    }

    LOG_DEBUG("Compiled and linked shader [ID: {}]", vertex);
    return true;
}

void Shader::use() const {
    glUseProgram(m_programID);
}

void Shader::setBool(const std::string& name, const bool value) const {
    glUniform1i(glGetUniformLocation(m_programID, name.c_str()), static_cast<int>(value));
}

void Shader::setInt(const std::string& name, const int value) const {
    glUniform1i(glGetUniformLocation(m_programID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, const float value) const {
    glUniform1f(glGetUniformLocation(m_programID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(glGetUniformLocation(m_programID, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(m_programID, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(m_programID, name.c_str()), 1, &value[0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(m_programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setTexture(const std::string& name, const GLuint textureID, const unsigned int slot, const GLenum target) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(target, textureID);
    setInt(name, static_cast<int>(slot));
}

bool Shader::checkCompileErrors(const GLuint shader, const ShaderCompilationType type) {
    GLint success;
    GLchar infoLog[1024];

    // Helper lambda to convert enum type to a readable label
    auto getTypeString = [](const ShaderCompilationType t) -> const char* {
        switch (t) {
            case Vertex:    return "VERTEX";
            case Fragment:  return "FRAGMENT";
            case Geometry:  return "GEOMETRY";
            default:        return "UNKNOWN";
        }
    };

    if (type != ShaderCompilationType::Program) {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
            LOG_ERROR("Shader compilation failed [{}]\n{}", getTypeString(type), infoLog);
            return false;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
            LOG_ERROR("Shader program linking failed\n{}", infoLog);
            return false;
        }
    }
    return true;
}