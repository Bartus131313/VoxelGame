#include "window.h"
#include <iostream>
#include <utility>

Window::Window(const int width, const int height, std::string  title)
    : m_width(width), m_height(height), m_title(std::move(title))
{
    init();
}

Window::~Window() {
    // If handle is not NULLPTR then destroy it
    if (m_handle) glfwDestroyWindow(m_handle);

    // Terminate GLFW
    glfwTerminate();
}

void Window::init() {
    // Check if GLFW was initialized successfully
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
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE); // Hidden until centered

    // Create GLFW Window handle with given size and title
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

    // Store 'this' pointer inside GLFW so static callbacks can access Window instance variables
    glfwSetWindowUserPointer(m_handle, this);
    glfwSetFramebufferSizeCallback(m_handle, framebufferSizeCallback);

    // Center the window and show it
    centerWindow();
    glfwShowWindow(m_handle);

    // Default clear color (BLACK)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void Window::centerWindow() {
    // Get primary monitor
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (!monitor) return;

    // Center the window based on the monitor work area size
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
    glfwPollEvents();
}

// Static Callback Router
void Window::framebufferSizeCallback(GLFWwindow* windowHandle, const int width, const int height) {
    // Retrieve the C++ Window object using User Pointer!
    if (auto* window = static_cast<Window*>(glfwGetWindowUserPointer(windowHandle))) {
        window->m_width = width;
        window->m_height = height;
        glViewport(0, 0, width, height);
    }
}