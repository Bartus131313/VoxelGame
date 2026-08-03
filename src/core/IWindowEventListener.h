#pragma once

/**
 * @brief Interface for receiving native window lifecycle and input events.
 *
 * Classes can inherit from this interface and override specific event handlers
 * to react to window state changes without needing individual callback pointers.
 */
class IWindowEventListener {
public:
    /** @brief Virtual destructor to ensure proper cleanup of derived listeners. */
    virtual ~IWindowEventListener() = default;

    /**
     * @brief Triggered when the window's dimensions or framebuffer size changes.
     *
     * @param width New width of the window in pixels.
     * @param height New height of the window in pixels.
     */
    virtual void onWindowResize(int width, int height) {}
};