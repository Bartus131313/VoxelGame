#include "KeyboardManager.h"
#include "Input.h"

void KeyboardManager::init(GLFWwindow* window) {
    m_window = window;
    glfwSetKeyCallback(m_window, keyCallback);
}

void KeyboardManager::update() {
    // Save current frame's keys to previous frame array
    m_previousKeys = m_currentKeys;
}

bool KeyboardManager::isKeyDown(const int key) const {
    // Safety check against invalid/unknown keys (e.g. GLFW_KEY_UNKNOWN is -1)
    if (key < 0 || key > GLFW_KEY_LAST) return false;
    return m_currentKeys[key];
}

bool KeyboardManager::isKeyJustPressed(const int key) const {
    if (key < 0 || key > GLFW_KEY_LAST) return false;
    return m_currentKeys[key] && !m_previousKeys[key];
}

bool KeyboardManager::isKeyJustReleased(const int key) const {
    if (key < 0 || key > GLFW_KEY_LAST) return false;
    return !m_currentKeys[key] && m_previousKeys[key];
}

// GLFW static C-callback router
void KeyboardManager::keyCallback(GLFWwindow* window, const int key, const int scancode,
    const int action, const int mods) {
    // Filter out unknown keypresses (-1) or invalid keycodes
    if (key < 0 || key > GLFW_KEY_LAST) return;

    KeyboardManager& keyboard = Input::getKeyboard();

    if (action == GLFW_PRESS) {
        keyboard.m_currentKeys[key] = true;
    } 
    else if (action == GLFW_RELEASE) {
        keyboard.m_currentKeys[key] = false;
    }
}