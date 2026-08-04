#pragma once

#include <memory>
#include <unordered_map>
#include <cstdint>

#include "../entity/Entity.h"
#include "../entity/impl/Player.h"
#include "../render/sky/SkyRenderer.h"

class World {
public:
    World();
    ~World();

    // Prevent accidental copying (World owns unique resources)
    World(const World&) = delete;
    World& operator=(const World&) = delete;

    // Allow moving
    World(World&&) noexcept = default;
    World& operator=(World&&) noexcept = default;

    /**
     * @brief Updates all entities and processes fixed game ticks.
     * @param deltaTime Time elapsed since the previous frame.
     */
    void update(float deltaTime);

    /**
     * @brief Renders all terrain, sky, and entities.
     * @param camera Active scene camera for view/projection matrices.
     */
    void render(const Camera3D& camera);

    /**
     * @brief Renders all terrain, sky, and entities using local player's camera if exists.
     */
    void render();

    /**
     * @brief Spawns a new entity in the world.
     * @tparam T Entity type deriving from Entity.
     * @tparam Args Constructor arguments for T.
     * @return Raw pointer to the newly created entity.
     */
    template<typename T, typename... Args>
    T* spawnEntity(Args&&... args) {
        auto entity = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = entity.get();
        m_entities[entity->getId()] = std::move(entity);
        return ptr;
    }

    /**
     * @brief Removes an entity by its unique identifier.
     */
    void removeEntity(const uint64_t entityId) {
        if (m_localPlayer && m_localPlayer->getId() == entityId) {
            m_localPlayer = nullptr; // Prevent dangling pointer
        }
        m_entities.erase(entityId);
    }

    /**
     * @brief Gets the Local Player reference without taking ownership.
     * @return Non-owning pointer to Player, or nullptr if not spawned.
     */
    [[nodiscard]] Player* getLocalPlayer() const { return m_localPlayer; }

private:
    /** @brief Executes fixed game ticks (physics, logic, world simulation). */
    void tick();

    /// Map of unique Entity ID -> Owning Entity Pointer
    std::unordered_map<uint64_t, std::unique_ptr<Entity>> m_entities;

    Player* m_localPlayer = nullptr;    ///< Non-owning raw pointer targeting the local player in m_entities.
    SkyRenderer m_skyRenderer{};        ///< Procedural sky environment.

    int64_t m_worldTicks{0};            ///< Total game ticks executed (fixed 20 Hz tick rate).
    float m_tickAccumulator{0.0f};      ///< Accumulated time for fixed timestep updates.

    static constexpr float TICKS_PER_SECOND = 20.0f; ///< How many ticks are in one second?
};