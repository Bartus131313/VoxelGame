#pragma once

#include "../render/camera/Camera3D.h"
#include <glm/glm.hpp>

/// Handles movement and camera rotation of local player.
class Player {
public:
    static constexpr float CAMERA_MOUSE_SENSITIVITY = 0.1f;
    static constexpr float CAMERA_GAMEPAD_SENSITIVITY = 1.2f;

    Player(glm::vec3 startPosition = glm::vec3(0.0f, 0.0f, 0.0f));

    /// Updates player position, velocity, physics, and synchronizes the camera.
    /// @param deltaTime Time elapsed since last frame.
    void update(float deltaTime);

    /// Returns Camera3D used by the player.
    /// @return Camera3D used to project everything on the screen.
    [[nodiscard]] Camera3D& getCamera() { return m_camera; }

    /// Returns world space position of the player.
    /// @return Position of the player in world space.
    [[nodiscard]] const glm::vec3& getPosition() const { return m_position; }

    /// Sets the player's position to the new one.
    /// @param position New position of the player.
    void setPosition(const glm::vec3& position) { m_position = position; }

private:
    glm::vec3 m_position{0.0f};     ///< World space position of the player.
    glm::vec3 m_velocity{0.0f};     ///< Velocity of the player.

    float m_eyeHeight{1.62f};       ///< Camera offset from player feet/center.
    float m_movementSpeed{5.0f};    ///< Movement speed of the player.

    Camera3D m_camera;      ///< Camera3D that the player owns.
};