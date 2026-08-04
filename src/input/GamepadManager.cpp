#include "GamepadManager.h"
#include <iostream>

#include "../core/Logger.h"

void GamepadManager::init() {
    glfwSetJoystickCallback(joystickCallback);

    // Initial check for any gamepads already connected at startup
    for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid) {
        if (glfwJoystickPresent(jid) && glfwJoystickIsGamepad(jid)) {
            m_gamepads[jid].connected = true;
        }
    }
}

void GamepadManager::update() {
    for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid) {
        if (!glfwJoystickPresent(jid) || !glfwJoystickIsGamepad(jid)) {
            m_gamepads[jid].connected = false;
            continue;
        }

        m_gamepads[jid].connected = true;

        // Copy current frame to previous frame history
        m_gamepads[jid].previousButtons = m_gamepads[jid].currentButtons;
        m_gamepads[jid].previousAxes = m_gamepads[jid].currentAxes;

        // Read new gamepad state from GLFW
        GLFWgamepadstate state;
        if (glfwGetGamepadState(jid, &state)) {
            // Update buttons
            for (int b = 0; b <= GLFW_GAMEPAD_BUTTON_LAST; ++b) {
                m_gamepads[jid].currentButtons[b] = (state.buttons[b] == GLFW_PRESS);
            }

            // Update axes
            for (int a = 0; a <= GLFW_GAMEPAD_AXIS_LAST; ++a) {
                m_gamepads[jid].currentAxes[a] = state.axes[a];
            }
        }
    }
}

bool GamepadManager::isConnected(const int gamepadID) const {
    if (gamepadID < GLFW_JOYSTICK_1 || gamepadID > GLFW_JOYSTICK_LAST) return false;
    return m_gamepads[gamepadID].connected;
}

bool GamepadManager::isButtonDown(const int button, const int gamepadID) const {
    if (!isConnected(gamepadID) || button < 0 || button > GLFW_GAMEPAD_BUTTON_LAST) return false;
    return m_gamepads[gamepadID].currentButtons[button];
}

bool GamepadManager::isButtonJustPressed(const int button, const int gamepadID) const {
    if (!isConnected(gamepadID) || button < 0 || button > GLFW_GAMEPAD_BUTTON_LAST) return false;
    return m_gamepads[gamepadID].currentButtons[button] && !m_gamepads[gamepadID].previousButtons[button];
}

bool GamepadManager::isButtonJustReleased(const int button, const int gamepadID) const {
    if (!isConnected(gamepadID) || button < 0 || button > GLFW_GAMEPAD_BUTTON_LAST) return false;
    return !m_gamepads[gamepadID].currentButtons[button] && m_gamepads[gamepadID].previousButtons[button];
}

glm::vec2 GamepadManager::getLeftStick(const float deadZone, const int gamepadID) const {
    if (!isConnected(gamepadID)) return glm::vec2(0.0f);

    const float rawX = m_gamepads[gamepadID].currentAxes[GLFW_GAMEPAD_AXIS_LEFT_X];
    const float rawY = m_gamepads[gamepadID].currentAxes[GLFW_GAMEPAD_AXIS_LEFT_Y];

    return applyRadialDeadZone(rawX, rawY, deadZone);
}

glm::vec2 GamepadManager::getRightStick(const float deadZone, const int gamepadID) const {
    if (!isConnected(gamepadID)) return glm::vec2(0.0f);

    const float rawX = m_gamepads[gamepadID].currentAxes[GLFW_GAMEPAD_AXIS_RIGHT_X];
    const float rawY = m_gamepads[gamepadID].currentAxes[GLFW_GAMEPAD_AXIS_RIGHT_Y];

    return applyRadialDeadZone(rawX, rawY, deadZone);
}

float GamepadManager::getLeftTrigger(const int gamepadID) const {
    if (!isConnected(gamepadID)) return 0.0f;
    const float raw = m_gamepads[gamepadID].currentAxes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER];
    return (raw + 1.0f) * 0.5f;
}

float GamepadManager::getRightTrigger(const int gamepadID) const {
    if (!isConnected(gamepadID)) return 0.0f;
    const float raw = m_gamepads[gamepadID].currentAxes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];
    return (raw + 1.0f) * 0.5f;
}

glm::vec2 GamepadManager::applyRadialDeadZone(const float x, const float y, const float deadZone) {
    const glm::vec2 stick(x, y);
    const float magnitude = glm::length(stick);

    if (magnitude < deadZone) {
        return glm::vec2(0.0f);
    }

    // Smoothly scale the input from dead zone threshold to 1.0
    const glm::vec2 dir = glm::normalize(stick);
    const float scaledMagnitude = (magnitude - deadZone) / (1.0f - deadZone);
    return dir * glm::clamp(scaledMagnitude, 0.0f, 1.0f);
}

void GamepadManager::joystickCallback(const int jid, const int event) {
    if (event == GLFW_CONNECTED) {
        if (glfwJoystickIsGamepad(jid)) {
            LOG_INFO("Gamepad connected: {} (Slot {})", glfwGetGamepadName(jid), jid);
        }
    } else if (event == GLFW_DISCONNECTED) {
        LOG_INFO("Gamepad disconnected (Slot {})", jid);
    }
}