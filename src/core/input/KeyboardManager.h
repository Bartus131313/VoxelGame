#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <array>

/// Handles keyboard state tracking, key history transitions, and key callback routing.
class KeyboardManager {
public:
    KeyboardManager() = default;

    /// Initializes the Keyboard Manager and attaches callbacks to the GLFW window.
    /// @param window Pointer to the active GLFW window context.
    void init(GLFWwindow* window);

    /// Updates previous key states for single-frame trigger queries.
    /// @note Call once per frame at the start of the update pass.
    void update();

    /// Checks whether a specific keyboard key is currently held down.
    /// @param key GLFW key code constant (e.g., @c GLFW_KEY_W, @c GLFW_KEY_SPACE).
    /// @return @c true if key is down, @c false otherwise.
    [[nodiscard]] bool isKeyDown(int key) const;

    /// Checks whether a specific keyboard key was pressed during the current frame.
    /// @param key GLFW key code constant (e.g., @c GLFW_KEY_W, @c GLFW_KEY_SPACE).
    /// @return @c true only on the initial press frame, @c false otherwise.
    [[nodiscard]] bool isKeyJustPressed(int key) const;

    /// Checks whether a specific keyboard key was released during the current frame.
    /// @param key GLFW key code constant (e.g., @c GLFW_KEY_W, @c GLFW_KEY_SPACE).
    /// @return @c true only on the release frame, @c false otherwise.
    [[nodiscard]] bool isKeyJustReleased(int key) const;

private:
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    GLFWwindow* m_window = nullptr; ///< Active GLFW window handle.

    std::array<bool, GLFW_KEY_LAST + 1> m_currentKeys{};  ///< Key states in current frame.
    std::array<bool, GLFW_KEY_LAST + 1> m_previousKeys{}; ///< Key states in previous frame.
};