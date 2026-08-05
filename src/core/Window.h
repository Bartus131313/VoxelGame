#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

#include "IWindowEventListener.h"

/** @brief Display mode for the window. */
enum class WindowMode {
    Windowed,       ///< Standard windowed mode with window decorations (title bar, borders).
    Borderless,     ///< Borderless fullscreen matching monitor resolution without mode changes.
    Fullscreen      ///< Exclusive hardware fullscreen mode with mode switching.
};

/** @brief Bitmask flags for Window creation options. */
enum class WindowFlags : uint32_t {
    None        = 0,
    Resizable   = 1 << 0,
    VSync       = 1 << 1,
    AlwaysOnTop = 1 << 2,
    Focused     = 1 << 3,

    Default     = VSync | Focused
};

constexpr WindowFlags operator|(WindowFlags a, WindowFlags b) {
    return static_cast<WindowFlags>(
        static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
    );
}

constexpr WindowFlags operator&(WindowFlags a, WindowFlags b) {
    return static_cast<WindowFlags>(
        static_cast<uint32_t>(a) & static_cast<uint32_t>(b)
    );
}

constexpr WindowFlags& operator|=(WindowFlags& a, const WindowFlags b) {
    a = a | b;
    return a;
}

/** @brief Wrapper around GLFW window lifecycle, OpenGL context creation, and frame operations. */
class Window {
public:
    /**
     * @brief Creates and initializes a GLFW window and OpenGL context.
     *
     * @param width Initial window width in pixels.
     * @param height Initial window height in pixels.
     * @param title Text displayed in the window title bar.
     * @param flags Window flags applied when creating OpenGL window (v-sync, focused, maximized, etc.)
     */
    Window(int width, int height, std::string title, WindowFlags flags = WindowFlags::Default);

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
     * @brief Checks whether the user or OS requested the window to close.
     *
     * @return @c true if window should close, @c false otherwise.
     */
    [[nodiscard]] bool shouldClose() const;


    /**
     * @brief Changes the display mode of the window.
     *
     * @param mode Desired display mode (@c Windowed, @c WindowedFullscreen, or @c Fullscreen).
     */
    void setWindowMode(WindowMode mode);

    /**
     * @brief Swaps front and back rendering buffers and processes OS window events.
     *
     * @note Call once per frame at the end of the frame cycle.
     */
    void update() const;

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

    /**
     * @brief Gets the window title.
     *
     * @return Title of the window.
     */
    [[nodiscard]] std::string getTitle() const { return m_title; }

    /**
     * @brief Gets the current display mode of the window.
     *
     * @return Current display mode.
     */
    [[nodiscard]] WindowMode getWindowMode() const { return m_mode; }

    /** @brief Closes window and frees all resources. */
    void close() const;

private:
    /** @brief Initialized the window with raw GLFW calls. */
    void init();
    /** @brief Centers the window. */
    void centerWindow() const;

    /** @brief Helper function to test if a specific flag is active. */
    static constexpr bool hasFlag(const WindowFlags flags, const WindowFlags flag) {
        return (flags & flag) == flag;
    }

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
    WindowFlags m_flags;            ///< Window flags applied when creating OpenGL window.

    WindowMode m_mode = WindowMode::Windowed;   ///< Active display mode.
    int m_windowedX = 0;                        ///< Saved window X coordinate when leaving windowed mode.
    int m_windowedY = 0;                        ///< Saved window Y coordinate when leaving windowed mode.
    int m_windowedWidth = 0;                    ///< Saved window width when leaving windowed mode.
    int m_windowedHeight = 0;                   ///< Saved window height when leaving windowed mode.

    /// List which holds all window event listeners.
    std::vector<IWindowEventListener*> m_listeners;
};