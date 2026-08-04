#include "Renderer.h"

#include <iostream>

#include "../../core/Logger.h"

void Renderer::init() {
    // Enable depth buffer testing for correct 3D spatial sorting
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Enable backface culling to optimize fragment processing
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Enable alpha channel blending for transparent textures and UI elements
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    LOG_INFO("Initialized global OpenGL states.");
}

void Renderer::clear(const glm::vec3& color) {
    glClearColor(color.r, color.g, color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::clear(const float r, const float g, const float b) {
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::setViewport(const int width, const int height) {
    glViewport(0, 0, width, height);
}

void Renderer::setDepthFunc(const GLenum func) {
    glDepthFunc(func);
}

void Renderer::setFaceCulling(const bool enable) {
    if (enable) glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);
}

void Renderer::setBlending(const bool enable) {
    if (enable) glEnable(GL_BLEND);
    else glDisable(GL_BLEND);
}

void Renderer::setBlendFunc(const GLenum sourceFactor, const GLenum destFactor) {
    glBlendFunc(sourceFactor, destFactor);
}

void Renderer::checkError(std::string_view context, const std::source_location location) {
    while (GLenum err = glGetError()) {
        if (context.empty()) {
            Logger::log(LogLevel::Warn, location, "OpenGL error: 0x{:04X}", err);
        } else {
            Logger::log(LogLevel::Warn, location, "OpenGL error after [{}]: 0x{:04X}", context, err);
        }
    }
}