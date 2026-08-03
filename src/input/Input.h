#pragma once

#include "KeyboardManager.h"
#include "MouseManager.h"
#include "GamepadManager.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

/// Directional directions for analog stick axes and triggers.
enum class GamepadAxisDir {
    LeftStickLeft,   ///< Left stick pushed LEFT  (X < -threshold)
    LeftStickRight,  ///< Left stick pushed RIGHT (X > +threshold)
    LeftStickUp,     ///< Left stick pushed UP    (Y < -threshold)
    LeftStickDown,   ///< Left stick pushed DOWN  (Y > +threshold)

    RightStickLeft,  ///< Right stick pushed LEFT  (X < -threshold)
    RightStickRight, ///< Right stick pushed RIGHT (X > +threshold)
    RightStickUp,    ///< Right stick pushed UP    (Y < -threshold)
    RightStickDown,  ///< Right stick pushed DOWN  (Y > +threshold)

    LeftTrigger,     ///< Left trigger pulled past threshold
    RightTrigger     ///< Right trigger pulled past threshold
};

/// Types of hardware inputs that can trigger an action.
enum class InputType {
    KeyboardKey,
    MouseButton,
    GamepadButton,
    GamepadAxis
};

/// Holds input type and code that needs to be activated for action.
struct ActionBinding {
    InputType type;
    int code = 0;                          ///< Key code or button constant.
    GamepadAxisDir axisDir{};              ///< Direction for GamepadAxis types.
    float threshold = 0.5f;                ///< Deflection threshold (0.0f to 1.0f).
};

/// Handles all input related logic for Keyboard, Mouse and Gamepads.
class Input {
public:
    /// Initializes all device managers and registers GLFW callbacks.
    /// @param window Pointer to the active GLFW window context.
    static void init(GLFWwindow* window);

    /// Polls device states and updates input history for all managers.
    /// @note Must be called once per frame at the beginning of the update pass.
    static void update();

    /// Gets the Keyboard Manager instance.
    /// @return Reference to the active KeyboardManager.
    static KeyboardManager& getKeyboard() { return s_keyboard; }

    /// Gets the Mouse Manager instance.
    /// @return Reference to the active MouseManager.
    static MouseManager& getMouse() { return s_mouse; }

    /// Gets the Gamepad Manager instance.
    /// @return Reference to the active GamepadManager.
    static GamepadManager& getGamepad() { return s_gamepad; }

    /**
     * @brief Registers a new logical action mapping in the input system.
     *
     * Actions group multiple physical input sources (keyboard keys, mouse buttons,
     * or gamepad buttons) under a single abstract name (e.g., "jump", "interact").
     *
     * @param actionName The unique identifier for the action.
     * @note If the action already exists, calling this will reset its bindings.
     */
    static void createAction(const std::string& actionName);
    /**
     * @brief Binds a physical keyboard key to a registered action.
     *
     * @param actionName The name of the target action created with createAction().
     * @param key The GLFW key code (e.g., @c GLFW_KEY_SPACE, @c GLFW_KEY_W).
     */
    static void bindKey(const std::string& actionName, int key);

    /**
     * @brief Binds a physical mouse button to a registered action.
     *
     * @param actionName The name of the target action created with createAction().
     * @param button The GLFW mouse button constant (e.g., @c GLFW_MOUSE_BUTTON_LEFT).
     */
    static void bindMouseButton(const std::string& actionName, int button);

    /**
     * @brief Binds a physical gamepad button to a registered action.
     *
     * @param actionName The name of the target action created with createAction().
     * @param gamepadButton The GLFW gamepad button constant (e.g., @c GLFW_GAMEPAD_BUTTON_A).
     */
    static void bindGamepadButton(const std::string& actionName, int gamepadButton);

    /**
     * @brief Binds an analog stick direction or trigger to an action.
     *
     * @param actionName The target action identifier.
     * @param direction The analog axis direction (e.g., @c GamepadAxisDir::LeftStickUp).
     * @param threshold Deflection sensitivity threshold from 0.0 to 1.0 (default: 0.5).
     */
    static void bindGamepadAxis(const std::string& actionName, GamepadAxisDir direction, float threshold = GamepadManager::DEFAULT_DEAD_ZONE);

    /**
     * @brief Checks whether an action is currently being held down.
     *
     * Returns true continuously for as long as at least one physical input
     * bound to @p actionName remains pressed.
     *
     * @param actionName The name of the action to query.
     * @param playerIndex Specific player index used for multiple players on one device. (Max index = 15)
     * @return @c true if any bound input is down, @c false otherwise.
     */
    static bool isActionPressed(const std::string& actionName, int playerIndex = 0);

    /**
     * @brief Checks whether an action was triggered on this exact frame.
     *
     * Useful for single-fire events like jumping, opening menus, or firing a weapon.
     *
     * @param actionName The name of the action to query.
     * @param playerIndex Specific player index used for multiple players on one device. (Max index = 15)
     * @return @c true only on the initial frame of activation, @c false on subsequent held frames.
     */
    static bool isActionJustPressed(const std::string& actionName, int playerIndex = 0);

    /**
     * @brief Checks whether an action was released on this exact frame.
     *
     * Useful for charging mechanics, releasing pulled bows, or stopping continuous sounds.
     *
     * @param actionName The name of the action to query.
     * @param playerIndex Specific player index used for multiple players on one device. (Max index = 15)
     * @return @c true only on the frame the physical input was released, @c false otherwise.
     */
    static bool isActionJustReleased(const std::string& actionName, int playerIndex = 0);

    /**
     * @brief Gets the analog strength (0.0 - 1.0) of an action.
     *
     * Digital sources (keyboard, mouse, gamepad buttons) return either 0.0f or 1.0f.
     * Gamepad stick/trigger sources return a value proportional to how far the stick
     * or trigger is deflected, remapped so that the configured dead-zone @c threshold
     * maps to 0.0 and full deflection maps to 1.0. If multiple bindings are attached
     * to the action, the strongest one wins.
     *
     * @param actionName The name of the action to query.
     * @param playerIndex Specific player index used for multiple players on one device. (Max index = 15)
     * @return A value from 0.0f (inactive) to 1.0f (fully active/deflected).
     */
    static float getActionStrength(const std::string& actionName, int playerIndex = 0);

    /**
     * @brief Calculates a normalized 2D movement vector from four directional actions.
     *
     * Combines opposing axis inputs (e.g., WASD or D-Pad) into a single directional vector.
     * Gamepad stick bindings contribute their analog deflection (via getActionStrength()),
     * so partial stick pushes yield a proportionally shorter vector, while keyboard/button
     * bindings contribute a full 1.0f. The resulting vector's length is clamped to 1.0f
     * (so diagonals don't exceed full speed) but is NOT force-normalized, preserving analog
     * partial magnitudes.
     *
     * @param left Action name for -X direction.
     * @param right Action name for +X direction.
     * @param up Action name for +Y direction.
     * @param down Action name for -Y direction.
     * @param playerIndex Specific player index used for multiple players on one device. (Max index = 15)
     * @return A @c glm::vec2 direction vector with magnitude in [0.0f, 1.0f].
     */
    static glm::vec2 getVector(const std::string& left, const std::string& right,
                               const std::string& up, const std::string& down, int playerIndex = 0);

    /// Helper to get the total number of connected gamepads.
    /// @return Total number of connected gamepads.
    static int getConnectedGamepadCount();

private:
    static bool isAxisDirectionActive(GamepadAxisDir dir, float threshold, int gamepadID, bool previousFrame = false);

    /// Helper to evaluate stick/trigger deflection strength, remapped from [threshold, 1] to [0, 1].
    static float getAxisDirectionStrength(GamepadAxisDir dir, float threshold, int gamepadID);

    static inline GLFWwindow* s_window = nullptr; ///< Active GLFW window context handle.

    // Subsystems
    static inline KeyboardManager s_keyboard;     ///< Keyboard state and key history manager.
    static inline MouseManager    s_mouse;        ///< Mouse cursor, delta, and scroll manager.
    static inline GamepadManager  s_gamepad;      ///< Gamepad polling, axis, and dead zone manager.

    // Action Storage
    static inline std::unordered_map<std::string, std::vector<ActionBinding>> s_actions; ///< Maps action names to hardware bindings.
};