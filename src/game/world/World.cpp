#include "World.h"

#include <ranges>

#include "../Game.h"
#include "../../core/Logger.h"
#include "../render/RenderSystem.h"

World::World() {
    // Initialize sky renderer
    m_skyRenderer.init(sunTextureLocation, moonAtlasLocation);

    // Spawn local player
    m_localPlayer = spawnEntity<Player>();
}

World::~World() {
    m_entities.clear();

    LOG_INFO("Cleaned up world.");
}

void World::update(const float deltaTime) {
    // Fixed-timestep tick loop
    m_tickAccumulator += deltaTime * Game::TICKS_PER_SECOND;
    while (m_tickAccumulator >= 1.0f) {
        tick();
        m_tickAccumulator -= 1.0f;
    }

    // Update sky renderer
    m_skyRenderer.update(m_worldTicks);

    // Update all entities
    for (const auto &entity: m_entities | std::views::values) {
        if (!entity) continue;

        entity->update(deltaTime);
    }
}

void World::tick() {
    m_worldTicks++;

    // TODO: Perform fixed 20 TPS logic here (block updates, chunk generation checks, etc.)
}

void World::render(const Camera3D& camera) {
    // Enter 3D rendering
    RenderSystem::enter3D();

    // Render skybox
    m_skyRenderer.render(camera.getViewMatrix(), camera.getProjectionMatrix());

    // Render all entities
    for (const auto &entity: m_entities | std::views::values) {
        if (!entity) continue;

        entity->render(camera);
    }
}

void World::render() {
    if (!m_localPlayer) {
        LOG_WARN("Attempted to render world without a local player.");
        return;
    }

    render(m_localPlayer->getCamera());
}