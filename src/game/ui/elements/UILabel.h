#pragma once

#include "../UIElement.h"
#include "../../render/font/FontRenderer.h"

#include <string>

/// A 2D UI element responsible for rendering text labels on screen using cached font resources.
class UILabel : public UIElement {
public:
    /// Constructs a UILabel with a specified font, text content, font size, and tint color.
    /// @param fontFilePath Relative path to the font file in the fonts' directory.
    /// @param fontSize Target height of the font in pixels.
    /// @param text Initial text content to render.
    /// @param color Initial RGBA color vector for text tinting (defaults to solid white).
    UILabel(const std::string& fontFilePath, const int fontSize, std::string text,
        const glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f})
        : m_fontData(FontManager::loadFont(fontFilePath, fontSize)), m_text(std::move(text)), m_color(color) {
    }

    /// Updates the label's string content.
    /// @param text New string content to render.
    void setText(const std::string& text) { m_text = text; }

    /// Updates the label's RGBA tint color and opacity.
    /// @param color New RGBA color vector.
    void setColor(const glm::vec4& color) { m_color = color; }

    /// Renders the text string using the provided shader program and cached font atlas.
    /// @param shader Active shader program configured for 2D UI projection.
    void render(const Shader& shader) override {
        FontRenderer::renderText(shader, m_fontData, m_text, m_position.x, m_position.y, m_scale, m_color);
    }

private:
    FontData* m_fontData;   ///< Pointer to cached font atlas metadata used for rendering.
    std::string m_text;     ///< Current string content to display.
    glm::vec4 m_color;      ///< RGBA tint color applied to rendered font glyphs.
};