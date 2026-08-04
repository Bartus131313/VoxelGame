#pragma once

#include <glm/glm.hpp>

#include "../Entity.h"
#include "../../render/camera/Camera3D.h"

/**
 * @brief Local, input-driven entity representing the player.
 *
 * Extends Entity so the future World/EntityManager can store and update it alongside
 * other world entities (mobs, dropped items, projectiles, ...) polymorphically, while
 * still owning player-specific concerns like the camera and mouse/keyboard input bindings.
 */
class Player : public Entity {
public:
    static constexpr float CAMERA_MOUSE_SENSITIVITY = 0.1f;     ///< Camera sensitivity for mouse.
    static constexpr float CAMERA_GAMEPAD_SENSITIVITY = 1.2f;   ///< Camera sensitivity for gamepad.

    /**
     * @brief Initialized Player with start position and Camera3D.
     *
     * @param startPosition Center/feet position of the player.
     */
    explicit Player(glm::vec3 startPosition = glm::vec3(0.0f, 0.0f, 0.0f));

    /**
     * @brief Updates player position, velocity, physics, and synchronizes the camera.
     *
     * @param deltaTime Time elapsed since last frame.
     */
    void update(float deltaTime) override;

    /**
     * @brief Returns Camera3D used by the player.
     *
     * @return Camera3D used to project everything on the screen.
     */
    [[nodiscard]] Camera3D& getCamera() { return m_camera; }

    /**
     * @brief Sets the player's position and immediately re-syncs the camera to match.
     *
     * Overridden so external callers (e.g. a future spawn/teleport command) see the
     * camera update instantly instead of waiting for the next update() call.
     *
     * @param position New position of the player.
     */
    void setPosition(const glm::vec3& position) override;

private:
    float m_eyeHeight{1.62f};       ///< Camera offset from player feet/center.
    float m_movementSpeed{5.0f};    ///< Movement speed of the player.

    Camera3D m_camera;      ///< Camera3D that the player owns.
};