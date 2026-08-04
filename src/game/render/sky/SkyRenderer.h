#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>

#include "../mesh/impl/SkyboxMesh.h"
#include "../texture/TextureManager.h"
#include "../shader/ShaderManager.h"

/**
 * @brief Renders a fully procedural sky with  a gradient dome plus
 * a sun and moon that arc across it based on the current game tick.
 *
 * Ticks follow that convention: 24000 ticks = 1 full day/night cycle,
 * tick 0 = dawn (sunrise on the eastern horizon), tick 6000 = noon (sun at zenith),
 * tick 12000 = dusk (sunset), tick 18000 = midnight (moon at zenith).
 */
class SkyRenderer {
public:
    SkyRenderer() = default;

    /**
     * @brief Loads shaders and textures required by the sky renderer.
     *
     * @param sunTexturePath Path (relative to @c TextureManager::TEXTURES_PATH) to the sun texture.
     * @param moonAtlasPath Path (relative to @c TextureManager::TEXTURES_PATH) to the moon phase atlas.
     */
    void init(const std::string& sunTexturePath, const std::string& moonAtlasPath);

    /**
     * @brief Advances the sky state to the given absolute game tick.
     * @param ticks Absolute game tick count (not delta).
     */
    void update(std::int64_t ticks);

    /**
     * @brief Renders the sky dome followed by the sun and moon.
     *
     * Should be called first in the render pass, before any opaque world geometry,
     * with depth writes conceptually "at infinity" (the shaders push depth to 1.0).
     *
     * @param view View matrix (translation will be stripped internally).
     * @param projection Projection matrix.
     */
    void render(const glm::mat4& view, const glm::mat4& projection) const;

    /** @brief Gets current fraction of the day cycle elapsed, in [0, 1). 0 = dawn, 0.25 = noon. */
    [[nodiscard]] float getDayProgress() const { return m_dayProgress; }

    /** @brief Gets current daylight factor in [0, 1]. 1 = full day, 0 = full night. */
    [[nodiscard]] float getDaylightFactor() const { return m_daylightFactor; }

private:
    /**
     * @brief Renders only procedural sky dome (gradient with stars at night).
     *
     * @param view View matrix.
     * @param projection Projection matrix.
     */
    void renderSkyDome(const glm::mat4& view, const glm::mat4& projection) const;

    /**
     *
     * @param view View matrix.
     * @param projection Projection matrix.
     * @param direction Direction which the body will be facing in.
     * @param angle Angle of the body.
     * @param texture Texture of the body (sun or moon).
     * @param uvMin Minimal UV coords of the texture.
     * @param uvMax Maximal UV coords of the texture.
     * @param size Size of the body.
     * @param brightness Brightness of the texture (alpha).
     */
    void renderCelestialBody(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& direction,
                             float angle, const std::shared_ptr<TextureData>& texture, glm::vec2 uvMin,
                             glm::vec2 uvMax, float size, float brightness) const;

    SkyboxMesh m_domeMesh{}; ///< Reused unit cube, sampled as a direction vector for the gradient dome.

    std::shared_ptr<Shader> m_domeShader = nullptr;         ///< Procedural gradient sky shader.
    std::shared_ptr<Shader> m_celestialShader = nullptr;    ///< Billboard quad shader for sun/moon.

    std::shared_ptr<TextureData> m_sunTexture = nullptr;    ///< Sun texture used by celestial body.
    std::shared_ptr<TextureData> m_moonAtlas = nullptr;     ///< All moon phases used by celestial body.

    GLuint m_celestialVAO = 0, m_celestialVBO = 0;          ///< Unit quad in local space, built once.

    static constexpr std::int64_t TICKS_PER_DAY = 24000;    ///< How many ticks are in one full day?

    float m_dayProgress = 0.0f;     ///< [0, 1) fraction through the current day. 0 = dawn.
    float m_daylightFactor = 1.0f;  ///< [0, 1] how "day-like" the sky is right now (for gradient + brightness).
    int m_moonPhase = 0;            ///< [0, 7] current moon phase index into the atlas.
    float m_elapsedSeconds = 0.0f;  ///< Derived from ticks; drives star twinkle animation only.
};