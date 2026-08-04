#include "Input.h"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "../core/Logger.h"

void Input::init(GLFWwindow* window) {
    s_window = window;
    s_keyboard.init(window);
    s_mouse.init(window);
    s_gamepad.init();
}

void Input::update() {
    s_keyboard.update();
    s_mouse.update();
    s_gamepad.update();

    glfwPollEvents();
}

void Input::createAction(const std::string& actionName) {
    if (s_actions.contains(actionName)) {
        LOG_WARN("Action `{}` already exists!", actionName);
        return;
    }

    s_actions[actionName] = std::vector<ActionBinding>();
}

void Input::bindKey(const std::string& actionName, const int key) {
    if (!s_actions.contains(actionName)) {
        LOG_WARN("Action `{}` does not exist!", actionName);
        return;
    }

    s_actions[actionName].push_back({ InputType::KeyboardKey, key });
}

void Input::bindMouseButton(const std::string& actionName, const int button) {
    if (!s_actions.contains(actionName)) {
        LOG_WARN("Action `{}` does not exist!", actionName);
        return;
    }

    s_actions[actionName].push_back({ InputType::MouseButton, button });
}

void Input::bindGamepadButton(const std::string& actionName, const int gamepadButton) {
    if (!s_actions.contains(actionName)) {
        LOG_WARN("Action `{}` does not exist!", actionName);
        return;
    }

    s_actions[actionName].push_back({ InputType::GamepadButton, gamepadButton });
}

void Input::bindGamepadAxis(const std::string& actionName, const GamepadAxisDir direction, const float threshold) {
    if (!s_actions.contains(actionName)) {
        LOG_WARN("Action `{}` does not exist!", actionName);
        return;
    }

    s_actions[actionName].push_back({ InputType::GamepadAxis, 0, direction, threshold });
}

int Input::getConnectedGamepadCount() {
    int count = 0;
    for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid) {
        if (s_gamepad.isConnected(jid)) {
            count++;
        }
    }
    return count;
}

bool Input::isActionPressed(const std::string& actionName, const int playerIndex) {
    const auto it = s_actions.find(actionName);
    if (it == s_actions.end()) {
        LOG_WARN("Action `{}` does not exist!", actionName);
        return false;
    }

    const int gamepadID = GLFW_JOYSTICK_1 + playerIndex;

    for (const ActionBinding binding : it->second) {
        switch (binding.type) {
            case InputType::KeyboardKey:
                if (playerIndex == 0 && s_keyboard.isKeyDown(binding.code)) return true;
                break;
            case InputType::MouseButton:
                if (playerIndex == 0 && s_mouse.isButtonDown(binding.code)) return true;
                break;
            case InputType::GamepadButton:
                if (s_gamepad.isButtonDown(binding.code, gamepadID)) return true;
                break;
            case InputType::GamepadAxis:
                if (isAxisDirectionActive(binding.axisDir, binding.threshold, gamepadID, false)) return true;
                break;
        }
    }
    return false;
}

bool Input::isActionJustPressed(const std::string& actionName, const int playerIndex) {
    const auto it = s_actions.find(actionName);
    if (it == s_actions.end()) {
        LOG_WARN("Action `{}` does not exist!", actionName);
        return false;
    }

    const int gamepadID = GLFW_JOYSTICK_1 + playerIndex;

    for (const ActionBinding binding : it->second) {
        switch (binding.type) {
            case InputType::KeyboardKey:
                if (playerIndex == 0 && s_keyboard.isKeyJustPressed(binding.code)) return true;
                break;
            case InputType::MouseButton:
                if (playerIndex == 0 && s_mouse.isButtonJustPressed(binding.code)) return true;
                break;
            case InputType::GamepadButton:
                if (s_gamepad.isButtonJustPressed(binding.code, gamepadID)) return true;
                break;
            case InputType::GamepadAxis: {
                const bool current  = isAxisDirectionActive(binding.axisDir, binding.threshold, gamepadID, false);
                const bool previous = isAxisDirectionActive(binding.axisDir, binding.threshold, gamepadID, true);
                if (current && !previous) return true;
                break;
            }
        }
    }
    return false;
}

bool Input::isActionJustReleased(const std::string& actionName, const int playerIndex) {
    const auto it = s_actions.find(actionName);
    if (it == s_actions.end()) {
        LOG_WARN("Action `{}` does not exist!", actionName);
        return false;
    }

    const int gamepadID = GLFW_JOYSTICK_1 + playerIndex;

    for (const ActionBinding binding : it->second) {
        switch (binding.type) {
            case InputType::KeyboardKey:
                if (playerIndex == 0 && s_keyboard.isKeyJustReleased(binding.code)) return true;
                break;
            case InputType::MouseButton:
                if (playerIndex == 0 && s_mouse.isButtonJustReleased(binding.code)) return true;
                break;
            case InputType::GamepadButton:
                if (s_gamepad.isButtonJustReleased(binding.code, gamepadID)) return true;
                break;
            case InputType::GamepadAxis: {
                const bool current  = isAxisDirectionActive(binding.axisDir, binding.threshold, gamepadID, false);
                const bool previous = isAxisDirectionActive(binding.axisDir, binding.threshold, gamepadID, true);
                if (!current && previous) return true;
                break;
            }
        }
    }

    return false;
}

float Input::getActionStrength(const std::string& actionName, const int playerIndex) {
    const auto it = s_actions.find(actionName);
    if (it == s_actions.end()) {
        LOG_WARN("Action `{}` does not exist!", actionName);
        return 0.0f;
    }

    const int gamepadID = GLFW_JOYSTICK_1 + playerIndex;
    float strength = 0.0f;

    for (const ActionBinding binding : it->second) {
        switch (binding.type) {
            case InputType::KeyboardKey:
                if (playerIndex == 0 && s_keyboard.isKeyDown(binding.code)) strength = std::max(strength, 1.0f);
                break;
            case InputType::MouseButton:
                if (playerIndex == 0 && s_mouse.isButtonDown(binding.code)) strength = std::max(strength, 1.0f);
                break;
            case InputType::GamepadButton:
                if (s_gamepad.isButtonDown(binding.code, gamepadID)) strength = std::max(strength, 1.0f);
                break;
            case InputType::GamepadAxis:
                strength = std::max(strength, getAxisDirectionStrength(binding.axisDir, binding.threshold, gamepadID));
                break;
        }
    }
    return strength;
}

glm::vec2 Input::getVector(const std::string& left, const std::string& right,
                           const std::string& up, const std::string& down,
                           const int playerIndex) {
    glm::vec2 dir(0.0f);
    if (!right.empty()) dir.x += getActionStrength(right, playerIndex);
    if (!left.empty())  dir.x -= getActionStrength(left, playerIndex);
    if (!up.empty())    dir.y += getActionStrength(up, playerIndex);
    if (!down.empty())  dir.y -= getActionStrength(down, playerIndex);

    // Only clamp when the combined magnitude would exceed 1 (e.g. two full-strength
    // digital inputs on a diagonal). Analog partial deflection keeps its true magnitude.
    const float length = glm::length(dir);
    if (length > 1.0f) {
        dir /= length;
    }
    return dir;
}

/// Helper to evaluate stick deflection against threshold
bool Input::isAxisDirectionActive(const GamepadAxisDir dir, const float threshold, const int gamepadID, const bool previousFrame) {
    if (!s_gamepad.isConnected(gamepadID)) return false;

    // GLFW Stick Y Convention: UP is negative (-1.0), DOWN is positive (+1.0)
    // GLFW Stick X Convention: LEFT is negative (-1.0), RIGHT is positive (+1.0)
    switch (dir) {
        case GamepadAxisDir::LeftStickLeft:
            return s_gamepad.getRawAxis(GLFW_GAMEPAD_AXIS_LEFT_X, previousFrame, gamepadID) < -threshold;
        case GamepadAxisDir::LeftStickRight:
            return s_gamepad.getRawAxis(GLFW_GAMEPAD_AXIS_LEFT_X, previousFrame, gamepadID) > threshold;
        case GamepadAxisDir::LeftStickUp:
            return s_gamepad.getRawAxis(GLFW_GAMEPAD_AXIS_LEFT_Y, previousFrame, gamepadID) < -threshold;
        case GamepadAxisDir::LeftStickDown:
            return s_gamepad.getRawAxis(GLFW_GAMEPAD_AXIS_LEFT_Y, previousFrame, gamepadID) > threshold;

        case GamepadAxisDir::RightStickLeft:
            return s_gamepad.getRawAxis(GLFW_GAMEPAD_AXIS_RIGHT_X, previousFrame, gamepadID) < -threshold;
        case GamepadAxisDir::RightStickRight:
            return s_gamepad.getRawAxis(GLFW_GAMEPAD_AXIS_RIGHT_X, previousFrame, gamepadID) > threshold;
        case GamepadAxisDir::RightStickUp:
            return s_gamepad.getRawAxis(GLFW_GAMEPAD_AXIS_RIGHT_Y, previousFrame, gamepadID) < -threshold;
        case GamepadAxisDir::RightStickDown:
            return s_gamepad.getRawAxis(GLFW_GAMEPAD_AXIS_RIGHT_Y, previousFrame, gamepadID) > threshold;

        case GamepadAxisDir::LeftTrigger:
            return s_gamepad.getLeftTrigger(gamepadID) > threshold;
        case GamepadAxisDir::RightTrigger:
            return s_gamepad.getRightTrigger(gamepadID) > threshold;
    }
    return false;
}

/// Helper to compute stick/trigger deflection strength in [0, 1], remapped so that
/// `threshold` maps to 0.0 and full deflection (1.0) maps to 1.0.
float Input::getAxisDirectionStrength(const GamepadAxisDir dir, const float threshold, const int gamepadID) {
    if (!s_gamepad.isConnected(gamepadID)) return 0.0f;

    // Remaps a raw magnitude in [0, 1] so that values <= threshold become 0,
    // and values from threshold..1 are rescaled to 0..1.
    const auto remap = [threshold](const float rawMagnitude) -> float {
        if (rawMagnitude <= threshold) return 0.0f;
        if (threshold >= 1.0f) return 0.0f; // avoid div-by-zero for degenerate thresholds
        const float t = (rawMagnitude - threshold) / (1.0f - threshold);
        return std::clamp(t, 0.0f, 1.0f);
    };

    switch (dir) {
        case GamepadAxisDir::LeftStickLeft: {
            const float v = s_gamepad.getRawAxis(GLFW_GAMEPAD_AXIS_LEFT_X, false, gamepadID);
            return v < 0.0f ? remap(-v) : 0.0f;
        }
        case GamepadAxisDir::LeftStickRight: {
            const float v = s_gamepad.getRawAxis(GLFW_GAMEPAD_AXIS_LEFT_X, false, gamepadID);
            return v > 0.0f ? remap(v) : 0.0f;
        }
        case GamepadAxisDir::LeftStickUp: {
            const float v = s_gamepad.getRawAxis(GLFW_GAMEPAD_AXIS_LEFT_Y, false, gamepadID);
            return v < 0.0f ? remap(-v) : 0.0f;
        }
        case GamepadAxisDir::LeftStickDown: {
            const float v = s_gamepad.getRawAxis(GLFW_GAMEPAD_AXIS_LEFT_Y, false, gamepadID);
            return v > 0.0f ? remap(v) : 0.0f;
        }

        case GamepadAxisDir::RightStickLeft: {
            const float v = s_gamepad.getRawAxis(GLFW_GAMEPAD_AXIS_RIGHT_X, false, gamepadID);
            return v < 0.0f ? remap(-v) : 0.0f;
        }
        case GamepadAxisDir::RightStickRight: {
            const float v = s_gamepad.getRawAxis(GLFW_GAMEPAD_AXIS_RIGHT_X, false, gamepadID);
            return v > 0.0f ? remap(v) : 0.0f;
        }
        case GamepadAxisDir::RightStickUp: {
            const float v = s_gamepad.getRawAxis(GLFW_GAMEPAD_AXIS_RIGHT_Y, false, gamepadID);
            return v < 0.0f ? remap(-v) : 0.0f;
        }
        case GamepadAxisDir::RightStickDown: {
            const float v = s_gamepad.getRawAxis(GLFW_GAMEPAD_AXIS_RIGHT_Y, false, gamepadID);
            return v > 0.0f ? remap(v) : 0.0f;
        }

        case GamepadAxisDir::LeftTrigger:
            return remap(s_gamepad.getLeftTrigger(gamepadID));
        case GamepadAxisDir::RightTrigger:
            return remap(s_gamepad.getRightTrigger(gamepadID));
    }
    return 0.0f;
}