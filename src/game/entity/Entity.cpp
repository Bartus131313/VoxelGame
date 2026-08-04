#include "Entity.h"

#include <atomic>

Entity::Entity(const glm::vec3 position)
    : m_position(position), m_id(generateId()) {
}

void Entity::update(const float deltaTime) {
    m_position += m_velocity * deltaTime;
}

std::uint64_t Entity::generateId() {
    // Monotonically increasing counter shared across all entities in the process.
    // Not thread-safe across multiple worlds/threads spawning entities concurrently yet,
    // but sufficient for the current single-threaded game loop.
    static std::atomic<std::uint64_t> nextId{1};
    return nextId.fetch_add(1, std::memory_order_relaxed);
}