#include "Window.h"

#include <iostream>
#include <utility>

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

void Window::init() {
    // Check if GLFW initializes successfully
    if (!glfwInit()) {
        std::cerr << "[Window] Failed to initialize GLFW!\n";
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
        std::cerr << "[Window] Failed to create GLFW window!\n";
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_handle);

    // Initialize GLAD function pointers
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "[Window] Failed to initialize GLAD!\n";
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
    glEnable(GL_DEPTH_TEST);
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

void Window::clear(const float r, const float g, const float b, const float a) const {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::update() const {
    glfwSwapBuffers(m_handle);
}

void Window::close() const {
    glfwSetWindowShouldClose(m_handle, true);
}

void Window::framebufferSizeCallback(GLFWwindow* windowHandle, const int width, int const height) {
    if (auto* window = static_cast<Window*>(glfwGetWindowUserPointer(windowHandle))) {
        window->m_width = width;
        window->m_height = height;
        glViewport(0, 0, width, height);
    }
}