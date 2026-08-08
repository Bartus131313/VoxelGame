#include "RenderSystem.h"

#include <iostream>

#include "../../core/Logger.h"

RenderMode RenderSystem::s_renderMode{Fill};

int RenderSystem::s_windowWidth = 0;
int RenderSystem::s_windowHeight = 0;

bool RenderSystem::s_initialized{false};

void RenderSystem::init(const int windowWidth, const int windowHeight) {
    if (s_initialized) return;
    s_initialized = true;

    // Enable depth buffer testing for correct 3D spatial sorting
    setDepthTest(true);
    setDepthFunc(DepthFunc::Less);

    // Enable backface culling to optimize fragment processing
    setFaceCulling(true);
    setCullFace(CullFace::Back);
    setWindingOrder(WindingOrder::CounterClockwise);

    // Enable alpha channel blending for transparent textures and UI elements
    setBlending(true);
    setBlendFunc(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha);

    setViewport(windowWidth, windowHeight);

    LOG_INFO("Initialized global OpenGL states. Framebuffer size: {}x{}", windowWidth, windowHeight);
}

void RenderSystem::clear(const glm::vec3& color) {
    glClearColor(color.r, color.g, color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderSystem::clear(const float r, const float g, const float b) {
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderSystem::setViewport(const int width, const int height) {
    glViewport(0, 0, width, height);

    s_windowWidth = width;
    s_windowHeight = height;
}

void RenderSystem::setDepthTest(const bool enable) {
    if (enable) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);
}

void RenderSystem::setDepthFunc(const DepthFunc func) {
    GLenum glFunc = GL_LESS;
    switch (func) {
        case DepthFunc::Never:          glFunc = GL_NEVER; break;
        case DepthFunc::Less:           glFunc = GL_LESS; break;
        case DepthFunc::Equal:          glFunc = GL_EQUAL; break;
        case DepthFunc::LessOrEqual:    glFunc = GL_LEQUAL; break;
        case DepthFunc::Greater:        glFunc = GL_GREATER; break;
        case DepthFunc::NotEqual:       glFunc = GL_NOTEQUAL; break;
        case DepthFunc::GreaterOrEqual: glFunc = GL_GEQUAL; break;
        case DepthFunc::Always:         glFunc = GL_ALWAYS; break;
    }
    glDepthFunc(glFunc);
}

void RenderSystem::setFaceCulling(const bool enable) {
    if (enable) glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);
}

void RenderSystem::setCullFace(const CullFace face) {
    switch (face) {
        case CullFace::Front:        glCullFace(GL_FRONT); break;
        case CullFace::Back:         glCullFace(GL_BACK); break;
        case CullFace::FrontAndBack: glCullFace(GL_FRONT_AND_BACK); break;
    }
}

void RenderSystem::setWindingOrder(const WindingOrder order) {
    switch (order) {
        case WindingOrder::Clockwise:        glFrontFace(GL_CW); break;
        case WindingOrder::CounterClockwise: glFrontFace(GL_CCW); break;
    }
}

void RenderSystem::setBlending(const bool enable) {
    if (enable) glEnable(GL_BLEND);
    else glDisable(GL_BLEND);
}

static GLenum toGLFactor(const BlendFactor factor) {
    switch (factor) {
        case BlendFactor::Zero:                  return GL_ZERO;
        case BlendFactor::One:                   return GL_ONE;
        case BlendFactor::SrcColor:              return GL_SRC_COLOR;
        case BlendFactor::OneMinusSrcColor:      return GL_ONE_MINUS_SRC_COLOR;
        case BlendFactor::DstColor:              return GL_DST_COLOR;
        case BlendFactor::OneMinusDstColor:      return GL_ONE_MINUS_DST_COLOR;
        case BlendFactor::SrcAlpha:              return GL_SRC_ALPHA;
        case BlendFactor::OneMinusSrcAlpha:      return GL_ONE_MINUS_SRC_ALPHA;
        case BlendFactor::DstAlpha:              return GL_DST_ALPHA;
        case BlendFactor::OneMinusDstAlpha:      return GL_ONE_MINUS_DST_ALPHA;
        case BlendFactor::ConstantColor:         return GL_CONSTANT_COLOR;
        case BlendFactor::OneMinusConstantColor: return GL_ONE_MINUS_CONSTANT_COLOR;
        case BlendFactor::ConstantAlpha:         return GL_CONSTANT_ALPHA;
        case BlendFactor::OneMinusConstantAlpha: return GL_ONE_MINUS_CONSTANT_ALPHA;
    }
    return GL_ONE;
}

void RenderSystem::setBlendFunc(const BlendFactor sourceFactor, const BlendFactor destFactor) {
    glBlendFunc(toGLFactor(sourceFactor), toGLFactor(destFactor));
}

void RenderSystem::setActiveTextureSlot(const uint32_t slot) {
    assert(slot < 32 && "Texture slot index exceeds maximum allowed texture units");
    glActiveTexture(GL_TEXTURE0 + slot);
}

void RenderSystem::bindTexture(const TextureType type, const GLuint textureId) {
    GLenum glTarget = GL_TEXTURE_2D;
    switch (type) {
        case TextureType::Texture2D:      glTarget = GL_TEXTURE_2D; break;
        case TextureType::Texture2DArray: glTarget = GL_TEXTURE_2D_ARRAY; break;
        case TextureType::TextureCubeMap: glTarget = GL_TEXTURE_CUBE_MAP; break;
        case TextureType::Texture3D:      glTarget = GL_TEXTURE_3D; break;
    }

    glBindTexture(glTarget, textureId);
}

void RenderSystem::bindTexture(const uint32_t slot, const TextureType type, const GLuint textureId) {
    setActiveTextureSlot(slot);
    bindTexture(type, textureId);
}

void RenderSystem::bindTexture(const uint32_t slot, const GLuint textureId) {
    bindTexture(slot, TextureType::Texture2D, textureId);
}

void RenderSystem::checkError(std::string_view context, const std::source_location location) {
    while (GLenum err = glGetError()) {
        if (context.empty()) {
            Logger::log(LogLevel::Warn, location, "OpenGL error: 0x{:04X}", err);
        } else {
            Logger::log(LogLevel::Warn, location, "OpenGL error after [{}]: 0x{:04X}", context, err);
        }
    }
}

void RenderSystem::enter2D() {
    setDepthTest(false);
    setFaceCulling(false);

    // Always force solid fill mode for UI elements and text
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void RenderSystem::enter3D() {
    setDepthTest(true);
    setFaceCulling(true);

    // Restore whatever render mode the 3D world is currently using
    setRenderMode(s_renderMode);
}

void RenderSystem::setRenderMode(const RenderMode mode) {
    s_renderMode = mode;

    switch (mode) {
        case Fill:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case Line:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        case Point:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            break;
    }
}