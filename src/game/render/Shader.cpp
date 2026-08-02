#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

Shader::~Shader() {
    if (m_programID != 0) glDeleteProgram(m_programID);
}

bool Shader::loadFromFile(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // Ensure ifstream objects can throw exceptions on failure
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (const std::ifstream::failure& e) {
        std::cerr << "[Shader] ERROR: Failed to read shader files from disk.\n"
                  << "  Vertex Path: " << vertexPath << "\n"
                  << "  Fragment Path: " << fragmentPath << "\n"
                  << "  Exception: " << e.what() << "\n";
        return false;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // Compile Vertex Shader
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    if (!checkCompileErrors(vertex, Vertex)) {
        glDeleteShader(vertex);
        return false;
    }

    // Compile Fragment Shader
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    if (!checkCompileErrors(fragment, Fragment)) {
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return false;
    }

    // Link Shader Program
    m_programID = glCreateProgram();
    glAttachShader(m_programID, vertex);
    glAttachShader(m_programID, fragment);
    glLinkProgram(m_programID);
    if (!checkCompileErrors(m_programID, Program)) {
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteProgram(m_programID);
        m_programID = 0;
        return false;
    }

    // Clean up shaders once linked into the program
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    std::cout << "[Shader] Successfully compiled and linked: " << vertexPath << " & " << fragmentPath << "\n";
    return true;
}

void Shader::use() const {
    glUseProgram(m_programID);
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(m_programID, name.c_str()), static_cast<int>(value));
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(m_programID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
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

bool Shader::checkCompileErrors(const GLuint shader, const ShaderCompilationType type) {
    GLint success;
    GLchar infoLog[1024];

    if (type != Program) {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "[Shader] ERROR: SHADER COMPILATION ERROR (" << type << ")\n"
                      << infoLog << "\n-------------------------------------------------\n";
            return false;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "[Shader] ERROR: PROGRAM LINKING ERROR\n"
                      << infoLog << "\n-------------------------------------------------\n";
            return false;
        }
    }
    return true;
}