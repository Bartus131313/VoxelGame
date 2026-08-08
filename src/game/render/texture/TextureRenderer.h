#pragma once
#include <memory>

#include "../../resource/ResourceLocation.h"
#include "../shader/Shader.h"
#include "glad/glad.h"

/** @brief Handles texture rendering on screen. */
class TextureRenderer {
public:
    /** @brief Creates the shared quad geometry and shader. Call once during renderer init. */
    static void init();

    /**
     * @brief Renders texture on screen with specified position and size.
     *
     * @param atlasName Name of the atlas where the texture is.
     * @param textureName Name of the texture in the atlas.
     * @param x X position of the texture.
     * @param y Y position of the texture.
     * @param width Width of the texture.
     * @param height Height of the texture.
     * @param color Color which the texture will be colored with. (Default: white)
     */
    static void renderTexture(const std::string& atlasName, const std::string& textureName, int x, int y,
        int width, int height, const glm::vec3& color = glm::vec3(1.0f, 1.0f, 1.0f));

    /** @brief Releases the shared quad geometry and shader. Call during shutdown. */
    static void cleanup();

private:
    /// ResourceLocation pointing to shader file.
    static const ResourceLocation shaderLocation;

    /// Shader used to blit texture on the screen.
    static std::shared_ptr<Shader> s_textureShader;

    static GLuint s_quadVAO;    ///< Vertex Array Object (VAO) of quad.
    static GLuint s_quadVBO;    ///< Vertex Buffer Object (VBO) of quad.

    static bool s_initialized;  ///< Is render already initialized?
};