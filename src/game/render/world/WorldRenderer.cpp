#include "WorldRenderer.h"

#include <ranges>

#include "../RenderSystem.h"

WorldRenderer::WorldRenderer() {

}

WorldRenderer::~WorldRenderer() {
    // Nothing need to be cleaned up here now
}

void WorldRenderer::init() {
    // Initialize sky renderer
    m_skyRenderer.init(sunTextureLocation, moonAtlasLocation);
}

void WorldRenderer::update(const WorldData& worldData, const float deltaTime) {
    // Update sky renderer
    m_skyRenderer.update(worldData.getWorldTicks());
}

void WorldRenderer::render(const WorldData& worldData, const Camera3D& camera) {
    // Enter 3D rendering
    RenderSystem::enter3D();

    // Render skybox
    m_skyRenderer.render(camera.getViewMatrix(), camera.getProjectionMatrix());

    // Render all entities
    for (const auto &entity: worldData.getEntities() | std::views::values) {
        if (!entity) continue;

        entity->render(camera);
    }
}

void WorldRenderer::render(const WorldData& worldData) {
    const Player* localPlayer = worldData.getLocalPlayer();
    if (!localPlayer) return;

    render(worldData, localPlayer->getCamera());
}