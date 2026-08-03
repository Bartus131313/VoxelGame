#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

#include "IWindowEventListener.h"

/** @brief Wrapper around GLFW window lifecycle, OpenGL context creation, and frame operations. */
class Window {
public:
    /**
     * @brief Creates and initializes a GLFW window and OpenGL context.
     *
     * @param width Initial window width in pixels.
     * @param height Initial window height in pixels.
     * @param title Text displayed in the window title bar.
     */
    Window(int width, int height, std::string  title);

    /** @brief Destroys the GLFW window handle and frees associated resources. */
    ~Window();

    // Prevent copying to enforce unique ownership of the GLFW window handle
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    // Allow move semantics for transferring window ownership
    Window(Window&& other) noexcept;
    Window& operator=(Window&& other) noexcept;

    // Single subscription method for ALL window events
    void addEventListener(IWindowEventListener* listener) {
        m_listeners.push_back(listener);
    }

    /**
     * @biref Checks whether the user or OS requested the window to close.
     *
     * @return @c true if window should close, @c false otherwise.
     */
    [[nodiscard]] bool shouldClose() const;

    /**
     * @biref Swaps front and back rendering buffers and processes OS window events.
     *
     * @note Call once per frame at the end of the frame cycle.
     */
    void update() const;

    /**
     * @biref Clears the framebuffer's color and depth buffers with a specified RGBA color.
     *
     * @param r Red component (0.0 to 1.0).
     * @param g Green component (0.0 to 1.0).
     * @param b Blue component (0.0 to 1.0).
     * @param a Alpha component (0.0 to 1.0).
     */
    void clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) const;

    /**
     * @brief Gets the current window width in pixels.
     *
     * @return Current window width.
     */
    [[nodiscard]] int getWidth() const { return m_width; }

    /**
     * @brief Gets the current window height in pixels.
     *
     * @return Current window height.
     */
    [[nodiscard]] int getHeight() const { return m_height; }

    /**
     * @brief Gets the raw underlying GLFW window handle.
     *
     * @return Pointer to native GLFW window handle.
     */
    [[nodiscard]] GLFWwindow* getNativeHandle() const { return m_handle; }

    /** @biref Closes window and frees all resources. */
    void close() const;

private:
    /** @biref Initialized the window with raw GLFW calls. */
    void init();
    /** @biref Centers the window. */
    void centerWindow() const;

    /**
     * @brief Retrieves the C++ Window instance associated with a native GLFW window handle.
     *
     * Helper function used inside static GLFW callbacks to extract the C++ instance
     * bound via @c glfwSetWindowUserPointer.
     *
     * @param handle Pointer to the native GLFW window handle.
     * @return Pointer to the associated C++ Window instance, or @c nullptr if not set.
     */
    static Window* getWindowInstance(GLFWwindow* handle);

    static void framebufferSizeCallback(GLFWwindow* windowHandle, int width, int height);

    GLFWwindow* m_handle = nullptr; ///< Native GLFW window handle.
    int m_width = 0;                ///< Current window width in pixels.
    int m_height = 0;               ///< Current window height in pixels.
    std::string m_title;            ///< Window title bar string.

    /// List which holds all window event listeners.
    std::vector<IWindowEventListener*> m_listeners;
};