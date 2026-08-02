#include "FontRenderer.h"

#include <vector>
#include <glad/glad.h>

#include "../Vertex.h"

void FontRenderer::renderText(const Shader& shader, const FontData* fontData, const std::string& text,
                              const int x, const int y, const float scale, const glm::vec4& color) {
    if (!fontData || text.empty()) return;

    // Enable Blending for font transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Bind Shader and Set Uniforms
    shader.setVec4("textColor", color);
    shader.setInt("textTexture", 0);

    // Bind Font Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fontData->textureID);

    // Setup initial layout coordinates with top-left anchor shift
    float currX = static_cast<float>(x);
    float currY = static_cast<float>(y) + (fontData->pixelAscent * scale);

    std::vector<Vertex2D> vertices;
    vertices.reserve(text.size() * 6); // 2 triangles per character (6 vertices)

    // Generate quad vertices for each character
    for (char c : text) {
        if (c < 32 || c >= 128) continue;

        stbtt_aligned_quad q;
        stbtt_GetPackedQuad(fontData->charData.data(), FontManager::FONT_ATLAS_WIDTH,
                            FontManager::FONT_ATLAS_HEIGHT, c - 32, &currX, &currY, &q, 1);

        // Triangle 1
        vertices.push_back({q.x0 * scale, q.y0 * scale, q.s0, q.t0});
        vertices.push_back({q.x1 * scale, q.y0 * scale, q.s1, q.t0});
        vertices.push_back({q.x1 * scale, q.y1 * scale, q.s1, q.t1});

        // Triangle 2
        vertices.push_back({q.x0 * scale, q.y0 * scale, q.s0, q.t0});
        vertices.push_back({q.x1 * scale, q.y1 * scale, q.s1, q.t1});
        vertices.push_back({q.x0 * scale, q.y1 * scale, q.s0, q.t1});
    }

    if (vertices.empty()) return;

    // Fast GPU Streaming into the persistent VBO managed by FontData
    glBindVertexArray(fontData->vao);
    glBindBuffer(GL_ARRAY_BUFFER, fontData->vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex2D), vertices.data(), GL_DYNAMIC_DRAW);

    // Draw call
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
    glBindVertexArray(0);
}