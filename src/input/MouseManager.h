#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <array>

/// Handles mouse state tracking, position deltas, cursor locking, and scroll events.
class MouseManager {
public:
    MouseManager() = default;

    /// Initializes the Mouse Manager and attaches callbacks to the GLFW window.
    /// @param window Pointer to the active GLFW window context.
    void init(GLFWwindow* window);

    /// Processes frame transitions and resets per-frame deltas.
    /// @note Call once per frame at the start of the update pass.
    void update();

    /// Locks (disables & hides) or unlocks the mouse cursor.
    /// @param locked @c true to capture and lock cursor to window, @c false to enable normal pointer.
    void setCursorLocked(bool locked);

    /// Checks whether the mouse cursor is currently captured and locked.
    /// @return @c true if cursor is locked, @c false otherwise.
    [[nodiscard]] bool isCursorLocked() const { return m_cursorLocked; }

    /// Gets the current absolute cursor position relative to the window.
    /// @return Current screen space position of the cursor in pixels.
    [[nodiscard]] glm::vec2 getPosition() const { return m_position; }

    /// Gets the mouse position delta (movement offset) since the last frame.
    /// @return 2D vector representing cursor movement delta (X: horizontal, Y: vertical).
    [[nodiscard]] glm::vec2 getDelta() const { return m_delta; }

    /// Gets the scroll wheel offset accumulated during the current frame.
    /// @return Scroll delta vector (Y component represents standard vertical scrolling).
    [[nodiscard]] glm::vec2 getScrollDelta() const { return m_scrollDelta; }

    /// Checks whether a specific mouse button is currently held down.
    /// @param button GLFW mouse button identifier (e.g., @c GLFW_MOUSE_BUTTON_LEFT).
    /// @return @c true if button is down, @c false otherwise.
    [[nodiscard]] bool isButtonDown(int button) const;

    /// Checks whether a specific mouse button was pressed during the current frame.
    /// @param button GLFW mouse button identifier (e.g., @c GLFW_MOUSE_BUTTON_LEFT).
    /// @return @c true only on the initial press frame, @c false otherwise.
    [[nodiscard]] bool isButtonJustPressed(int button) const;

    /// Checks whether a specific mouse button was released during the current frame.
    /// @param button GLFW mouse button identifier (e.g., @c GLFW_MOUSE_BUTTON_LEFT).
    /// @return @c true only on the release frame, @c false otherwise.
    [[nodiscard]] bool isButtonJustReleased(int button) const;

private:
    static void cursorPosCallback(GLFWwindow* window, double xPos, double yPos);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);

    GLFWwindow* m_window = nullptr;             ///< Active GLFW window handle.

    glm::vec2 m_position{0.0f, 0.0f};       ///< Current absolute position of cursor.
    glm::vec2 m_delta{0.0f, 0.0f};          ///< Movement offset since last frame.
    glm::vec2 m_scrollDelta{0.0f, 0.0f};    ///< Accumulation of scroll input for current frame.

    bool m_firstMouse = true;                   ///< Flag to prevent sudden camera jump on initial focus.
    bool m_cursorLocked = false;                ///< Tracks current cursor lock state.

    std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> m_currentButtons{};    ///< Button states in current frame.
    std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> m_previousButtons{};   ///< Button states in previous frame.
};