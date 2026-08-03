#include "Player.h"

#include <algorithm>
#include <iostream>
#include <ostream>

#include "../../input/Input.h"

Player::Player(glm::vec3 startPosition)
        : m_position(startPosition), m_camera(startPosition + glm::vec3(0.0f, m_eyeHeight, 0.0f)) {
    // Setup all movement actions
    Input::createAction("move_forward");
    Input::bindKey("move_forward", GLFW_KEY_W);
    Input::bindGamepadAxis("move_forward", GamepadAxisDir::LeftStickUp);

    Input::createAction("move_backward");
    Input::bindKey("move_backward", GLFW_KEY_S);
    Input::bindGamepadAxis("move_backward", GamepadAxisDir::LeftStickDown);

    Input::createAction("move_right");
    Input::bindKey("move_right", GLFW_KEY_D);
    Input::bindGamepadAxis("move_right", GamepadAxisDir::LeftStickRight);

    Input::createAction("move_left");
    Input::bindKey("move_left", GLFW_KEY_A);
    Input::bindGamepadAxis("move_left", GamepadAxisDir::LeftStickLeft);

    Input::createAction("move_up");
    Input::bindKey("move_up", GLFW_KEY_E);
    Input::bindGamepadButton("move_up", GLFW_GAMEPAD_BUTTON_DPAD_UP);

    Input::createAction("move_down");
    Input::bindKey("move_down", GLFW_KEY_Q);
    Input::bindGamepadButton("move_down", GLFW_GAMEPAD_BUTTON_DPAD_DOWN);

    Input::createAction("sprint");
    Input::bindKey("sprint", GLFW_KEY_LEFT_SHIFT);
    Input::bindGamepadButton("sprint", GLFW_GAMEPAD_BUTTON_LEFT_BUMPER);
}

void Player::update(const float deltaTime) {
    // Do not update player if cursor is unlocked
    if (!Input::getMouse().isCursorLocked()) return;

    // Handle player movement
    const glm::vec2 inputDir = Input::getVector("move_left", "move_right",
        "move_forward", "move_backward");

    const glm::vec2 verticalInputDir = Input::getVector("", "", "move_up", "move_down");

    auto moveDir = glm::vec3(0.0f, 0.0f, 0.0f);
    moveDir += m_camera.getFront() * inputDir.y;
    moveDir += m_camera.getRight() * inputDir.x;
    moveDir += Camera3D::WORLD_UP * verticalInputDir.y;

    m_position += moveDir * m_movementSpeed * deltaTime;

    // Handle camera rotation
    const glm::vec2 mouseDelta = Input::getMouse().getDelta();
    m_camera.yaw += mouseDelta.x * CAMERA_MOUSE_SENSITIVITY;
    m_camera.pitch += mouseDelta.y * CAMERA_MOUSE_SENSITIVITY;

    // Check if any gamepad is connected to support rotation using right stick
    if (Input::getGamepad().isConnected(GLFW_JOYSTICK_1)) {
        const glm::vec2 rightStickVector = Input::getGamepad().getRightStick();
        m_camera.yaw += rightStickVector.x * CAMERA_GAMEPAD_SENSITIVITY;
        m_camera.pitch -= rightStickVector.y * CAMERA_GAMEPAD_SENSITIVITY;
    }

    // Clamp pitch to prevent flipping and NaN vector math
    m_camera.pitch = std::clamp(m_camera.pitch, -89.0f, 89.0f);

    m_camera.updateVectors();

    // Keep the camera locked to the player's head/eyes
    m_camera.setPosition(m_position + glm::vec3(0.0f, m_eyeHeight, 0.0f));
}