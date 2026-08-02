#pragma once

#include <string>
#include <glm/glm.hpp>
#include "FontManager.h"
#include "../Shader.h"

/// Handles rendering of text using cached FontData and optimized GPU streaming.
class FontRenderer {
public:
    /// Renders text to the screen using the top-left anchor point.
    /// Shader must be bound before calling this function with @c projection uniform set.
    /// @param shader The active text shader.
    /// @param fontData Pointer to the loaded font data from FontManager.
    /// @param text The string to render.
    /// @param x Screen X coordinate (pixels).
    /// @param y Screen Y coordinate (pixels).
    /// @param scale Scaling factor for the text size.
    /// @param color RGBA color of the text.
    static void renderText(const Shader& shader, const FontData* fontData, const std::string& text,
                           int x, int y, float scale, const glm::vec4& color);
};