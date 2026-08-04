#include "Window.h"

#include <iostream>
#include <utility>

#include "Logger.h"

Window::Window(const int width, const int height, std::string  title)
    : m_width(width), m_height(height), m_title(std::move(title))
{
    init();
}

Window::~Window() {
    if (m_handle) glfwDestroyWindow(m_handle);
    glfwTerminate();
}

Window::Window(Window&& other) noexcept
    : m_handle(other.m_handle), m_width(other.m_width), m_height(other.m_height), m_title(std::move(other.m_title))
{
    // Leave moved-from object in a valid, empty state
    other.m_handle = nullptr;
    other.m_width = 0;
    other.m_height = 0;

    // Re-bind GLFW user pointer to 'this' new memory location
    if (m_handle) glfwSetWindowUserPointer(m_handle, this);
}

Window& Window::operator=(Window&& other) noexcept {
    if (this != &other) {
        // Destroy current resource if held
        if (m_handle) {
            glfwDestroyWindow(m_handle);
        }

        m_handle = other.m_handle;
        m_width = other.m_width;
        m_height = other.m_height;
        m_title = std::move(other.m_title);

        other.m_handle = nullptr;
        other.m_width = 0;
        other.m_height = 0;

        // Re-bind GLFW user pointer to 'this' new memory location
        if (m_handle) glfwSetWindowUserPointer(m_handle, this);
    }
    return *this;
}

Window* Window::getWindowInstance(GLFWwindow* handle) {
    return static_cast<Window*>(glfwGetWindowUserPointer(handle));
}

void Window::init() {
    // Check if GLFW initializes successfully
    if (!glfwInit()) {
        LOG_ERROR("Failed to initialize GLFW!");
        return;
    }

    // Configure OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

    // Create GLFW window handle
    m_handle = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (!m_handle) {
        LOG_ERROR("Failed to create GLFW window!");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_handle);

    // Initialize GLAD function pointers
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        LOG_ERROR("Failed to initialize GLAD!");
        return;
    }

    // Store 'this' pointer in GLFW so static callbacks can route to instance members
    glfwSetWindowUserPointer(m_handle, this);
    glfwSetFramebufferSizeCallback(m_handle, framebufferSizeCallback);

    // Center window and reveal it
    centerWindow();
    glfwShowWindow(m_handle);

    // Default clear state
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Enable depth test flag for proper depth sorting
    glEnable(GL_DEPTH_TEST);

    // Enable cull face flag to not render back side of the meshes
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void Window::centerWindow() const {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (!monitor) return;

    int monitorWidth, monitorHeight;
    glfwGetMonitorWorkarea(monitor, nullptr, nullptr, &monitorWidth, &monitorHeight);
    glfwSetWindowPos(m_handle, (monitorWidth - m_width) / 2, (monitorHeight - m_height) / 2);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_handle);
}

void Window::setWindowMode(const WindowMode mode) {
    if (m_mode == mode) return;

    // Get primary monitor using GLFW
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (!monitor) return;

    // Get video mode supported by this monitor
    const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
    if (!videoMode) return;

    // Save windowed bounds before leaving Windowed mode
    if (m_mode == WindowMode::Windowed) {
        glfwGetWindowPos(m_handle, &m_windowedX, &m_windowedY);
        glfwGetWindowSize(m_handle, &m_windowedWidth, &m_windowedHeight);
    }

    m_mode = mode;

    switch (m_mode) {
        case WindowMode::Windowed: {
            // Re-enable window borders and restore windowed monitor target
            glfwSetWindowAttrib(m_handle, GLFW_DECORATED, GLFW_TRUE);
            glfwSetWindowMonitor(
                m_handle,
                nullptr,
                m_windowedX,
                m_windowedY,
                m_windowedWidth,
                m_windowedHeight,
                GLFW_DONT_CARE
            );
            break;
        }

        case WindowMode::Borderless: {
            int monitorX = 0, monitorY = 0;
            glfwGetMonitorPos(monitor, &monitorX, &monitorY);

            // Exit exclusive fullscreen state if active
            glfwSetWindowMonitor(
                m_handle,
                nullptr,
                monitorX,
                monitorY,
                videoMode->width,
                videoMode->height,
                GLFW_DONT_CARE
            );

            // Remove window title bar & borders, then fit to monitor dimensions
            glfwSetWindowAttrib(m_handle, GLFW_DECORATED, GLFW_FALSE);
            glfwSetWindowPos(m_handle, monitorX, monitorY);
            glfwSetWindowSize(m_handle, videoMode->width, videoMode->height);
            break;
        }

        case WindowMode::Fullscreen: {
            // Re-enable decoration flag for when returning to windowed mode later
            glfwSetWindowAttrib(m_handle, GLFW_DECORATED, GLFW_TRUE);

            // Exclusive Hardware Fullscreen mode
            glfwSetWindowMonitor(
                m_handle,
                monitor,
                0,
                0,
                videoMode->width,
                videoMode->height,
                videoMode->refreshRate
            );
            break;
        }
    }
}

void Window::update() const {
    glfwSwapBuffers(m_handle);
}

void Window::close() const {
    glfwSetWindowShouldClose(m_handle, true);
}

void Window::framebufferSizeCallback(GLFWwindow* windowHandle, const int width, int const height) {
    auto* window = getWindowInstance(windowHandle);
    if (!window) return;

    window->m_width = width;
    window->m_height = height;

    // Forward to all registered listeners
    for (auto* listener : window->m_listeners) {
        listener->onWindowResize(width, height);
    }
}