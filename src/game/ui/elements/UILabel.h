#pragma once

#include "../UIElement.h"
#include "../../render/font/FontRenderer.h"

#include <string>

#include "../../../core/Logger.h"
#include "../../render/shader/ShaderManager.h"

/** @brief A 2D UI element responsible for rendering text labels on screen using cached font resources. */
class UILabel : public UIElement {
public:
    /**
     * @brief Constructs a UILabel with a specified font, text content, font size, and tint color.
     *
     * @param fontFileLocation ResourceLocation pointing to the font file.
     * @param fontSize Target height of the font in pixels.
     * @param text Initial text content to render.
     * @param color Initial RGBA color vector for text tinting (defaults to solid white).
     */
    UILabel(const ResourceLocation& fontFileLocation, const int fontSize, std::string text,
        const glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f})
        : m_fontData(FontManager::loadFont(fontFileLocation, fontSize)), m_text(std::move(text)), m_color(color) {
        m_shader = ShaderManager::loadShader(fontShaderLocation);
    }

    /**
     * @brief Updates the label's string content.
     *
     * @param text New string content to render.
     */
    void setText(const std::string& text) { m_text = text; }

    /**
     * @brief Updates the label's RGBA tint color and opacity.
     *
     * @param color New RGBA color vector.
     */
    void setColor(const glm::vec4& color) { m_color = color; }

    /**
     * @brief Renders the text string using the provided shader program and cached font atlas.
     *
     * @param projectionMatrix Matrix used to project the element on 2D screen.
     */
    void render(const glm::mat4 projectionMatrix) override {
        if (!m_shader->isValid()) return;

        // Use font shader and set projection matrix
        m_shader->use();
        m_shader->setMat4("uProjection", projectionMatrix);

        // Render text on screen
        FontRenderer::renderText(m_shader.get(), m_fontData.get(), m_text, m_position.x, m_position.y, m_scale, m_color);
    }

private:
    const ResourceLocation fontShaderLocation{"shaders/ui/blit_text"};

    std::shared_ptr<Shader> m_shader;       ///< Shader program used for rendering text.
    std::shared_ptr<FontData> m_fontData;   ///< Shared pointer to cached font atlas metadata used for rendering.
    std::string m_text;                     ///< Current string content to display.
    glm::vec4 m_color;                      ///< RGBA tint color applied to rendered font glyphs.
};