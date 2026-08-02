#include "Input.h"

#include <iostream>

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
        std::cerr << "[Input::createAction] Action already exists: " << actionName << std::endl;
        return;
    }

    s_actions[actionName] = std::vector<ActionBinding>();
}

void Input::bindKey(const std::string& actionName, const int key) {
    if (!s_actions.contains(actionName)) {
        std::cerr << "[Input::bindKey] Action does not exist: " << actionName << std::endl;
        return;
    }

    s_actions[actionName].push_back({ InputType::KeyboardKey, key });
}

void Input::bindMouseButton(const std::string& actionName, const int button) {
    if (!s_actions.contains(actionName)) {
        std::cerr << "[Input::bindMouseButton] Action does not exist: " << actionName << std::endl;
        return;
    }

    s_actions[actionName].push_back({ InputType::MouseButton, button });
}

void Input::bindGamepadButton(const std::string& actionName, const int gamepadButton) {
    if (!s_actions.contains(actionName)) {
        std::cerr << "[Input::bindGamepadButton] Action does not exist: " << actionName << std::endl;
        return;
    }

    s_actions[actionName].push_back({ InputType::GamepadButton, gamepadButton });
}

void Input::bindGamepadAxis(const std::string& actionName, const GamepadAxisDir direction, const float threshold) {
    if (!s_actions.contains(actionName)) {
        std::cerr << "[Input::bindGamepadAxis] Action does not exist: " << actionName << std::endl;
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
        std::cerr << "[Input::isActionPressed] Action does not exist: " << actionName << std::endl;
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
        std::cerr << "[Input::isActionJustPressed] Action does not exist: " << actionName << std::endl;
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
            case InputType::GamepadAxis:
                const bool current  = isAxisDirectionActive(binding.axisDir, binding.threshold, gamepadID, false);
                const bool previous = isAxisDirectionActive(binding.axisDir, binding.threshold, gamepadID, true);
                if (current && !previous) return true;
                break;
        }
    }
    return false;
}

bool Input::isActionJustReleased(const std::string& actionName, const int playerIndex) {
    const auto it = s_actions.find(actionName);
    if (it == s_actions.end()) {
        std::cerr << "[Input::isActionJustReleased] Action does not exist: " << actionName << std::endl;
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
            case InputType::GamepadAxis:
                const bool current  = isAxisDirectionActive(binding.axisDir, binding.threshold, gamepadID, false);
                const bool previous = isAxisDirectionActive(binding.axisDir, binding.threshold, gamepadID, true);
                if (!current && previous) return true;
                break;
        }
    }

    return false;
}

glm::vec2 Input::getVector(const std::string& left, const std::string& right,
                           const std::string& up, const std::string& down,
                           const int playerIndex) {
    glm::vec2 dir(0.0f);
    if (isActionPressed(right, playerIndex)) dir.x += 1.0f;
    if (isActionPressed(left,  playerIndex)) dir.x -= 1.0f;
    if (isActionPressed(up,    playerIndex)) dir.y += 1.0f;
    if (isActionPressed(down,  playerIndex)) dir.y -= 1.0f;

    if (glm::length(dir) > 0.0f) {
        dir = glm::normalize(dir);
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