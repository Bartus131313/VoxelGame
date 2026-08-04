#include "SkyRenderer.h"

#include <cmath>
#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/constants.hpp>

#include "../../Game.h"
#include "../shader/ShaderManager.h"

namespace {
    /// Unit quad, centered at origin, in local billboard space [-0.5, 0.5]
    constexpr float kQuadVertices[] = {
        // Position       // UV
        -0.5f, -0.5f,     0.0f, 0.0f,
         0.5f, -0.5f,     1.0f, 0.0f,
         0.5f,  0.5f,     1.0f, 1.0f,

         0.5f,  0.5f,     1.0f, 1.0f,
        -0.5f,  0.5f,     0.0f, 1.0f,
        -0.5f, -0.5f,     0.0f, 0.0f,
    };
}

void SkyRenderer::init(const std::string& sunTexturePath, const std::string& moonAtlasPath) {
    // Load all shaders first
    m_domeShader = ShaderManager::loadShader("environment/sky_dome");
    m_celestialShader = ShaderManager::loadShader("environment/celestial");

    // Then load all textures
    m_sunTexture = TextureManager::loadTexture(sunTexturePath, false);
    m_moonAtlas = TextureManager::loadTexture(moonAtlasPath, false);

    // Generate VAO and VBO
    glGenVertexArrays(1, &m_celestialVAO);
    glGenBuffers(1, &m_celestialVBO);

    // Send vertex data to VAO
    glBindVertexArray(m_celestialVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_celestialVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kQuadVertices), kQuadVertices, GL_STATIC_DRAW);

    // Setup vertex attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), static_cast<void*>(nullptr));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                           reinterpret_cast<void*>(2 * sizeof(float)));

    // Unbind VAO
    glBindVertexArray(0);
}

void SkyRenderer::update(const std::int64_t ticks) {
    // Calculate day progress based on total game ticks
    const std::int64_t tickOfDay = ((ticks % TICKS_PER_DAY) + TICKS_PER_DAY) % TICKS_PER_DAY;
    m_dayProgress = static_cast<float>(tickOfDay) / static_cast<float>(TICKS_PER_DAY);

    // Calculate sun angle and sun height using day progress
    const float sunAngle = m_dayProgress * glm::two_pi<float>(); // 0 at dawn, matches renderSkyDome/render math below
    const float sunHeight = std::sin(sunAngle); // -1 at midnight, +1 at noon, matches render()

    // This multiplier controls how long the transition between day and night is.
    // Smaller value means longer transition between day and night.
    constexpr float factorMultiplier = 8.0f;
    m_daylightFactor = glm::clamp(sunHeight * factorMultiplier + 0.5f, 0.0f, 1.0f);

    // Calculate current moon phase as it cycles once every 8 days.
    const std::int64_t dayCount = ticks / TICKS_PER_DAY;
    m_moonPhase = static_cast<int>(((dayCount % 8) + 8) % 8);

    // This value is only used to animate star twinkling.
    m_elapsedSeconds = static_cast<float>(ticks) / Game::TICKS_PER_SECOND;
}

void SkyRenderer::render(const glm::mat4& view, const glm::mat4& projection) const {
    //
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);

    renderSkyDome(view, projection);

    // SkyboxMesh::render() (used by renderSkyDome via m_domeMesh) resets glDepthFunc back to
    // GL_LESS and re-enables face culling internally when it finishes. Without re-asserting
    // GL_LEQUAL + no-cull here, the sun/moon quads below — which are also pushed to depth 1.0 —
    // fail the depth test against the dome's already-written depth of 1.0 (1.0 is not "less than"
    // 1.0) and get silently discarded, even though the draw call itself succeeds with no GL error.
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);

    // Compute sun/moon directions from the current day progress.
    // sunAngle = 0 -> dawn (sun on eastern horizon), pi/2 -> noon (zenith), pi -> dusk, 3pi/2 -> midnight (nadir).
    const float sunAngle = m_dayProgress * glm::two_pi<float>();
    const glm::vec3 sunDir = glm::normalize(glm::vec3(0.0f, std::sin(sunAngle), -std::cos(sunAngle)));
    const glm::vec3 moonDir = -sunDir; // Moon is always opposite the sun

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Standard alpha blend using the texture's real alpha channel

    // Sun: full-size atlas (no phases), full brightness, only really visible while above horizon
    const float sunBrightness = glm::clamp(sunDir.y * 4.0f + 0.2f, 0.0f, 1.0f);
    if (sunBrightness > 0.0f) {
        renderCelestialBody(view, projection, sunDir, sunAngle, m_sunTexture, glm::vec2(0.0f), glm::vec2(1.0f),
                             60.0f, sunBrightness);
    }

    // Moon: pick the UV rect for the current phase out of a 4x2 atlas
    const int col = m_moonPhase % 4;
    const int row = m_moonPhase / 4;
    const glm::vec2 moonUvMin(col / 4.0f, row / 2.0f);
    const glm::vec2 moonUvMax((col + 1) / 4.0f, (row + 1) / 2.0f);

    // Moon is always opposite the sun, which is the same as being half a rotation further along
    // the same arc — so its orientation angle is simply sunAngle + pi.
    const float moonAngle = sunAngle + glm::pi<float>();

    const float moonBrightness = glm::clamp(moonDir.y * 4.0f + 0.2f, 0.0f, 1.0f) * 0.6f;
    if (moonBrightness > 0.0f) {
        renderCelestialBody(view, projection, moonDir, moonAngle, m_moonAtlas, moonUvMin, moonUvMax, 48.0f, moonBrightness);
    }

    glDisable(GL_BLEND);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

}

void SkyRenderer::renderSkyDome(const glm::mat4& view, const glm::mat4& projection) const {
    if (!m_domeShader) return;

    const float sunAngle = m_dayProgress * glm::two_pi<float>();
    const glm::vec3 sunDir = glm::normalize(glm::vec3(0.0f, std::sin(sunAngle), -std::cos(sunAngle)));

    m_domeShader->use();
    m_domeShader->setMat4("view", view);
    m_domeShader->setMat4("projection", projection);
    m_domeShader->setFloat("daylightFactor", m_daylightFactor);
    m_domeShader->setVec3("sunDirection", sunDir);
    m_domeShader->setFloat("time", m_elapsedSeconds);

    // Tweak these palettes to taste; they roughly follow Minecraft's day/night sky colors.
    m_domeShader->setVec3("zenithDayColor", glm::vec3(0.30f, 0.55f, 0.95f));
    m_domeShader->setVec3("horizonDayColor", glm::vec3(0.70f, 0.82f, 0.95f));
    // Minecraft's night sky is essentially pure black, just barely lifted so the stars have
    // something to sit against without banding.
    // Minecraft's night sky is very dark, but not pure black — a faint dark-navy tint gives
    // the moon and stars something to actually contrast against instead of vanishing into void.
    m_domeShader->setVec3("zenithNightColor", glm::vec3(0.02f, 0.025f, 0.06f));
    m_domeShader->setVec3("horizonNightColor", glm::vec3(0.05f, 0.055f, 0.10f));

    m_domeMesh.render();
}

void SkyRenderer::renderCelestialBody(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& direction,
                                       const float angle, const std::shared_ptr<TextureData>& texture,
                                       const glm::vec2 uvMin, const glm::vec2 uvMax, const float size,
                                       const float brightness) const {
    if (!m_celestialShader) {
        std::cout << "[SkyRenderer] renderCelestialBody: skipped, no celestial shader\n";
        return;
    }
    if (!texture || !texture->isValid()) {
        std::cout << "[SkyRenderer] renderCelestialBody: skipped, texture missing/invalid\n";
        return;
    }

    constexpr float distance = 100.0f; // Arbitrary, just needs to sit inside the far plane
    const glm::vec3 worldPos = direction * distance;

    // Orient the quad as if it were rigidly bolted to a wheel rotating about the fixed world
    // X axis (the axis perpendicular to the sun/moon's arc), the same way Minecraft does it,
    // rather than deriving right/up per-frame from the direction vector via cross products.
    // A "look-at" billboard built from cross products is mathematically guaranteed to rotate
    // 180 degrees in-plane as direction sweeps from dawn to dusk (a straight-line reversal),
    // and that rotation's midpoint lands right at the zenith — which is exactly the flip that
    // was visible before. A rigid single-axis rotation has no such singularity: right stays
    // fixed (it lies along the rotation axis) and up rotates smoothly and continuously with
    // angle, so the sprite never appears to flip or spin, at the top of the arc or anywhere else.
    const glm::vec3 right(1.0f, 0.0f, 0.0f);
    const glm::vec3 up(0.0f, std::cos(angle), std::sin(angle));

    m_celestialShader->use();
    m_celestialShader->setMat4("view", view);
    m_celestialShader->setMat4("projection", projection);
    m_celestialShader->setVec3("worldPosition", worldPos);
    m_celestialShader->setVec3("right", right);
    m_celestialShader->setVec3("up", up);
    m_celestialShader->setFloat("size", size);
    m_celestialShader->setFloat("brightness", brightness);
    m_celestialShader->setVec2("uvMin", uvMin);
    m_celestialShader->setVec2("uvMax", uvMax);
    m_celestialShader->setTexture("bodyTexture", texture->id, 0, GL_TEXTURE_2D);

    glBindVertexArray(m_celestialVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    if (const GLenum err = glGetError(); err != GL_NO_ERROR) {
        std::cout << "[SkyRenderer] GL error after celestial draw: 0x" << std::hex << err << std::dec << "\n";
    }
}