#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <array>

/** @brief Handles gamepad polling, button histories, normalized triggers, and analog stick dead zones. */
class GamepadManager {
public:
    /** @brief Default dead zone threshold for analog sticks. */
    static constexpr float DEFAULT_DEAD_ZONE = 0.15f;

    GamepadManager() = default;

    /** @brief Initializes Gamepad Manager and hooks hotplug callbacks. */
    void init();

    /**
     * @brief Polls gamepad hardware states and updates button state histories.
     *
     * @note Call once per frame at the start of the update pass.
     */
    void update();

    /**
     * @brief Checks whether a gamepad with the given joystick ID is currently connected.
     *
     * @param gamepadID GLFW joystick identifier (e.g., @c GLFW_JOYSTICK_1).
     *
     * @return @c true if gamepad is connected, @c false otherwise.
     */
    [[nodiscard]] bool isConnected(int gamepadID = GLFW_JOYSTICK_1) const;

    /**
     * @brief Checks whether a gamepad button is currently held down.
     *
     * @param button GLFW gamepad button constant (e.g., @c GLFW_GAMEPAD_BUTTON_A).
     * @param gamepadID GLFW joystick identifier (e.g., @c GLFW_JOYSTICK_1).
     *
     * @return @c true if button is down, @c false otherwise.
     */
    [[nodiscard]] bool isButtonDown(int button, int gamepadID = GLFW_JOYSTICK_1) const;

    /**
     * @brief Checks whether a gamepad button was pressed during the current frame.
     *
     * @param button GLFW gamepad button constant (e.g., @c GLFW_GAMEPAD_BUTTON_A).
     * @param gamepadID GLFW joystick identifier (e.g., @c GLFW_JOYSTICK_1).
     *
     * @return @c true only on the initial press frame, @c false otherwise.
     */
    [[nodiscard]] bool isButtonJustPressed(int button, int gamepadID = GLFW_JOYSTICK_1) const;

    /**
     * @brief Checks whether a gamepad button was released during the current frame.
     *
     * @param button GLFW gamepad button constant (e.g., @c GLFW_GAMEPAD_BUTTON_A).
     * @param gamepadID GLFW joystick identifier (e.g., @c GLFW_JOYSTICK_1).
     *
     * @return @c true only on the release frame, @c false otherwise.
     */
    [[nodiscard]] bool isButtonJustReleased(int button, int gamepadID = GLFW_JOYSTICK_1) const;

    /**
     * @brief Gets a 2D direction vector for the Left Stick (-1.0 to 1.0) with scaled radial dead zone.
     *
     * @param deadZone Radial dead zone threshold (0.0 to 1.0).
     * @param gamepadID GLFW joystick identifier (e.g., @c GLFW_JOYSTICK_1).
     *
     * @return Normalized 2D analog stick vector.
     */
    [[nodiscard]] glm::vec2 getLeftStick(float deadZone = DEFAULT_DEAD_ZONE, int gamepadID = GLFW_JOYSTICK_1) const;

    /**
     * @brief Gets a 2D direction vector for the Right Stick (-1.0 to 1.0) with scaled radial dead zone.
     *
     *  @param deadZone Radial dead zone threshold (0.0 to 1.0).
     *  @param gamepadID GLFW joystick identifier (e.g., @c GLFW_JOYSTICK_1).
     *
     *  @return Normalized 2D analog stick vector.
     */
    [[nodiscard]] glm::vec2 getRightStick(float deadZone = DEFAULT_DEAD_ZONE, int gamepadID = GLFW_JOYSTICK_1) const;

    /**
     * @brief Gets raw value of Axis for current or previous frame.
     *
     *  @param axis Axis identifier.
     *  @param previousFrame Should raw value come from previous frame?
     *  @param gamepadID GLFW joystick identifier (e.g., @c GLFW_JOYSTICK_1).
     *
     *  @return Raw value of Axis.
     */
    [[nodiscard]] float getRawAxis(const int axis, const bool previousFrame = false, const int gamepadID = GLFW_JOYSTICK_1) const {
        if (!isConnected(gamepadID) || axis < 0 || axis > GLFW_GAMEPAD_AXIS_LAST) return 0.0f;
        return previousFrame ? m_gamepads[gamepadID].previousAxes[axis] : m_gamepads[gamepadID].currentAxes[axis];
    }

    /**
     * @brief Gets normalized Left Trigger press value.
     *
     * @param gamepadID GLFW joystick identifier (e.g., @c GLFW_JOYSTICK_1).
     *
     * @return Trigger position re-mapped to [0.0, 1.0] (0.0 = released, 1.0 = fully pressed).
     */
    [[nodiscard]] float getLeftTrigger(int gamepadID = GLFW_JOYSTICK_1) const;

    /**
     * @brief Gets normalized Right Trigger press value.
     *
     * @param gamepadID GLFW joystick identifier (e.g., @c GLFW_JOYSTICK_1).
     *
     * @return Trigger position re-mapped to [0.0, 1.0] (0.0 = released, 1.0 = fully pressed).
     */
    [[nodiscard]] float getRightTrigger(int gamepadID = GLFW_JOYSTICK_1) const;

private:
    static void joystickCallback(int jid, int event);

    /** @brief Holds internal state and input history for a single gamepad. */
    struct GamepadState {
        bool connected = false; ///< Connection state of this joystick slot.

        std::array<bool, GLFW_GAMEPAD_BUTTON_LAST + 1> currentButtons{};  ///< Button states in current frame.
        std::array<bool, GLFW_GAMEPAD_BUTTON_LAST + 1> previousButtons{}; ///< Button states in previous frame.
        std::array<float, GLFW_GAMEPAD_AXIS_LAST + 1> currentAxes{};      ///< Current axis values (-1.0 to 1.0).
        std::array<float, GLFW_GAMEPAD_AXIS_LAST + 1> previousAxes{};     ///< Previous axis values (-1.0 to 1.0).
    };

    std::array<GamepadState, GLFW_JOYSTICK_LAST + 1> m_gamepads{}; ///< States for all 16 GLFW joystick slots.

    /**
     * @brief Applies smooth radial dead zone scaling to raw analog stick input.
     *
     * @param x Raw horizontal stick axis input (-1.0 to 1.0).
     * @param y Raw vertical stick axis input (-1.0 to 1.0).
     * @param deadZone Dead zone threshold below which input is zeroed out.
     *
     * @return Scaled 2D direction vector.
     */
    [[nodiscard]] static glm::vec2 applyRadialDeadZone(float x, float y, float deadZone);
};