#include "WorldData.h"

#include <ranges>

#include "../Game.h"
#include "../../core/Logger.h"

WorldData::WorldData() {
    // Spawn local player
    m_localPlayer = spawnEntity<Player>();
}

WorldData::~WorldData() {
    // Clear entity map
    m_entities.clear();

    LOG_INFO("Cleaned up world data.");
}

void WorldData::update(const float deltaTime) {
    // Fixed-timestep tick loop
    m_tickAccumulator += deltaTime * Game::TICKS_PER_SECOND;
    while (m_tickAccumulator >= 1.0f) {
        tick();
        m_tickAccumulator -= 1.0f;
    }

    // Update all entities
    for (const auto &entity: m_entities | std::views::values) {
        if (!entity) continue;

        entity->update(deltaTime);
    }
}

void WorldData::tick() {
    m_worldTicks++;

    // TODO: Perform fixed 20 TPS logic here (block updates, chunk generation checks, etc.)
}