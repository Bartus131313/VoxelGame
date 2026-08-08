#include "TextureRenderer.h"

#include "TextureAtlasManager.h"
#include "../RenderSystem.h"
#include "../shader/Shader.h"
#include "../shader/ShaderManager.h"
#include "../../../core/Logger.h"

GLuint TextureRenderer::s_quadVAO = 0;
GLuint TextureRenderer::s_quadVBO = 0;
bool TextureRenderer::s_initialized = false;

const ResourceLocation TextureRenderer::shaderLocation{"shaders/ui/blit_texture"};
std::shared_ptr<Shader> TextureRenderer::s_textureShader = nullptr;

void TextureRenderer::init() {
    if (s_initialized) return;

    s_textureShader = ShaderManager::loadShader(shaderLocation);

    // Unit quad: position.xy (0..1), local uv.xy (0..1).
    // Actual on-screen position/size and atlas UV sub-rect are applied in the shader
    // via uniforms, so this single buffer is reused for every renderTexture() call.
    constexpr float vertices[] = {
        // pos       // uv
        0.0f, 0.0f,  0.0f, 0.0f,
        1.0f, 0.0f,  1.0f, 0.0f,
        1.0f, 1.0f,  1.0f, 1.0f,

        0.0f, 0.0f,  0.0f, 0.0f,
        1.0f, 1.0f,  1.0f, 1.0f,
        0.0f, 1.0f,  0.0f, 1.0f,
    };

    glGenVertexArrays(1, &s_quadVAO);
    glGenBuffers(1, &s_quadVBO);

    glBindVertexArray(s_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, s_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    s_initialized = true;
}

void TextureRenderer::cleanup() {
    if (!s_initialized) return;

    glDeleteBuffers(1, &s_quadVBO);
    glDeleteVertexArrays(1, &s_quadVAO);
    s_quadVBO = 0;
    s_quadVAO = 0;
    s_initialized = false;
}

void TextureRenderer::renderTexture(const std::string& atlasName, const std::string& textureName,
    const int x, const int y, const int width, const int height, const glm::vec3& color) {
    if (!s_initialized) {
        LOG_ERROR("TextureRenderer::renderTexture called before TextureRenderer::init()!");
        return;
    }

    if (s_textureShader == nullptr) return;

    // Get atlas OpenGL texture ID from manager
    const GLuint atlasTextureID = TextureAtlasManager::getAtlasTextureID(atlasName);
    if (atlasTextureID == 0) return;

    // Get texture UV box
    const auto [uMin, vMin, uMax, vMax] = TextureAtlasManager::getUVBox(atlasName, textureName);

    RenderSystem::enter2D();

    // Enable Blending for font transparency
    RenderSystem::setBlending(true);
    RenderSystem::setBlendFunc(BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha);

    s_textureShader->use();

    s_textureShader->setMat4("uProjection", RenderSystem::getOrthoProjection());
    s_textureShader->setVec2("uPosition", glm::vec2(static_cast<float>(x), static_cast<float>(y)));
    s_textureShader->setVec2("uSize", glm::vec2(static_cast<float>(width), static_cast<float>(height)));
    s_textureShader->setVec4("uUVBox", glm::vec4(uMin, vMin, uMax, vMax));
    s_textureShader->setInt("uTexture", 0);
    s_textureShader->setVec3("uColor", color);

    RenderSystem::bindTexture(0, TextureType::Texture2D, atlasTextureID);

    glBindVertexArray(s_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}