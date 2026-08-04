#pragma once

#include <cstdint>
#include <glm/glm.hpp>

/**
 * @brief Base class for anything that exists in the game world with a position and velocity.
 *
 * Serves as the common foundation for the local Player, mobs/NPCs, dropped item
 * entities, and projectiles. Provides identity, spatial state, and a virtual update hook so
 * a World/EntityManager can store and tick heterogeneous entities polymorphically
 * (e.g. via @c std::vector<std::unique_ptr<Entity>>).
 */
class Entity {
public:
    /**
     * @brief Constructs an Entity at the given world space position.
     *
     * @param position Initial world space position of the entity.
     */
    explicit Entity(glm::vec3 position = glm::vec3(0.0f));

    /** @brief Virtual destructor to allow safe polymorphic destruction of derived entities. */
    virtual ~Entity() = default;

    // Prevent copying to keep entity identity (m_id) unique
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;

    // Allow moving so entities can live in containers (e.g. std::vector) that reallocate
    Entity(Entity&&) noexcept = default;
    Entity& operator=(Entity&&) noexcept = default;

    /**
     * @brief Updates the entity's state for the current frame/tick.
     *
     * Base implementation only integrates velocity into position; derived entities
     * (Player, mobs, ...) should override this to add movement logic, physics, AI, etc.,
     * and may still call @c Entity::update() to keep the default integration behavior.
     *
     * @param deltaTime Time elapsed since the last update in seconds.
     */
    virtual void update(float deltaTime);

    /**
     * @brief Returns the unique runtime identifier of this entity.
     *
     * Useful for a future EntityManager/World to track, look up, or network-sync entities.
     *
     * @return Unique entity ID, stable for the lifetime of the entity instance.
     */
    [[nodiscard]] std::uint64_t getId() const { return m_id; }

    /**
     * @brief Returns the world space position of the entity.
     *
     * @return Position of the entity in world space.
     */
    [[nodiscard]] const glm::vec3& getPosition() const { return m_position; }

    /**
     * @brief Sets the world space position of the entity.
     *
     * @param position New position of the entity.
     */
    virtual void setPosition(const glm::vec3& position) { m_position = position; }

    /**
     * @brief Returns the current velocity of the entity.
     *
     * @return Velocity vector in world units per second.
     */
    [[nodiscard]] const glm::vec3& getVelocity() const { return m_velocity; }

    /**
     * @brief Sets the velocity of the entity.
     *
     * @param velocity New velocity in world units per second.
     */
    void setVelocity(const glm::vec3& velocity) { m_velocity = velocity; }

    /**
     * @brief Marks the entity for removal from the world on the next cleanup pass.
     *
     * Actual removal (e.g. erase from an EntityManager's container) is the world's
     * responsibility; this only flags intent so update/render passes can skip dead entities.
     */
    void markForRemoval() { m_alive = false; }

    /**
     * @brief Returns whether the entity is still alive and should be updated/rendered.
     *
     * @return @c true if the entity has not been marked for removal.
     */
    [[nodiscard]] bool isAlive() const { return m_alive; }

protected:
    glm::vec3 m_position{0.0f};   ///< World space position of the entity.
    glm::vec3 m_velocity{0.0f};   ///< Current velocity of the entity in world units per second.

private:
    /**
     * @brief Generates a fresh, process-unique entity ID.
     *
     * @return Next available entity ID.
     */
    static std::uint64_t generateId();

    std::uint64_t m_id;     ///< Unique runtime identifier of this entity.
    bool m_alive{true};     ///< Whether the entity is still considered alive/active.
};