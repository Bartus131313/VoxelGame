#include "MouseManager.h"
#include "Input.h"

void MouseManager::init(GLFWwindow* window) {
    m_window = window;

    // Hook callbacks to GLFW
    glfwSetCursorPosCallback(m_window, cursorPosCallback);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
    glfwSetScrollCallback(m_window, scrollCallback);
}

void MouseManager::update() {
    // Copy current frame's buttons to previous frame for 'just pressed' check
    m_previousButtons = m_currentButtons;

    // Reset per-frame deltas
    m_delta = {0.0f, 0.0f};
    m_scrollDelta = {0.0f, 0.0f};
}

void MouseManager::setCursorLocked(const bool locked) {
    m_cursorLocked = locked;
    glfwSetInputMode(m_window, GLFW_CURSOR, locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

    // Reset firstMouse flag so mouse doesn't jump violently when cursor locks
    if (locked) m_firstMouse = true;
}

bool MouseManager::isButtonDown(const int button) const {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return false;
    return m_currentButtons[button];
}

bool MouseManager::isButtonJustPressed(const int button) const {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return false;
    return m_currentButtons[button] && !m_previousButtons[button];
}

bool MouseManager::isButtonJustReleased(const int button) const {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return false;
    return !m_currentButtons[button] && m_previousButtons[button];
}

void MouseManager::cursorPosCallback(GLFWwindow* window, const double xPos, const double yPos) {
    // Route to static Input facade's mouse manager instance
    MouseManager& mouse = Input::getMouse();

    const auto currentPos = glm::vec2(static_cast<float>(xPos), static_cast<float>(yPos));

    if (mouse.m_firstMouse) {
        mouse.m_position = currentPos;
        mouse.m_firstMouse = false;
    }

    // Calculate position change since last frame
    mouse.m_delta.x += currentPos.x - mouse.m_position.x;
    mouse.m_delta.y += mouse.m_position.y - currentPos.y; // Invert Y so up is positive
    mouse.m_position = currentPos;
}

void MouseManager::mouseButtonCallback(GLFWwindow* window, const int button, const int action, const int mods) {
    MouseManager& mouse = Input::getMouse();

    if (button >= 0 && button <= GLFW_MOUSE_BUTTON_LAST) {
        if (action == GLFW_PRESS) {
            mouse.m_currentButtons[button] = true;
        } else if (action == GLFW_RELEASE) {
            mouse.m_currentButtons[button] = false;
        }
    }
}

void MouseManager::scrollCallback(GLFWwindow* window, const double xOffset, const double yOffset) {
    MouseManager& mouse = Input::getMouse();
    mouse.m_scrollDelta = glm::vec2(static_cast<float>(xOffset), static_cast<float>(yOffset));
}