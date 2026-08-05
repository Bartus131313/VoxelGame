#pragma once
#include "../../world/WorldData.h"
#include "../sky/SkyRenderer.h"

class WorldRenderer {
public:
    WorldRenderer();
    ~WorldRenderer();

    // Prevent accidental copying (WorldRenderer owns unique resources)
    WorldRenderer(const WorldRenderer&) = delete;
    WorldRenderer& operator=(const WorldRenderer&) = delete;

    // Allow moving
    WorldRenderer(WorldRenderer&&) noexcept = default;
    WorldRenderer& operator=(WorldRenderer&&) noexcept = default;

    /**
     * @brief Updates all essential renderers inside this one.
     *
     * @param worldData WorldData used to get all important data for updates.
     * @param deltaTime Time elapsed since the previous frame.
     */
    void update(const WorldData& worldData, float deltaTime);

    /**
     * @brief Renders all terrain, sky, and entities.
     *
     * @param worldData WorldData used to be rendered.
     * @param camera Active scene camera for view/projection matrices.
     */
    void render(const WorldData& worldData, const Camera3D& camera);

    /**
     * @brief Renders all terrain, sky, and entities using local player's camera if exists.
     *
     * @param worldData WorldData used to be rendered.
     */
    void render(const WorldData& worldData);

private:
    const ResourceLocation sunTextureLocation{"textures/environment/celestial/sun.png"};
    const ResourceLocation moonAtlasLocation{"textures/environment/celestial/moon_phases.png"};

    SkyRenderer m_skyRenderer{};        ///< Procedural sky environment.
};
