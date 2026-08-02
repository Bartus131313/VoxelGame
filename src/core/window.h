#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class Window {
public:
    Window(int width, int height, std::string  title);
    ~Window();

    // Prevent copying
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    // Allow moving if window ownership needs to be transferred
    Window(Window&& other) noexcept;
    Window& operator=(Window&& other) noexcept;

    // Main loop helpers
    bool shouldClose() const;
    void update() const; // Swaps buffers & polls events
    void clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) const;

    // Getters
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    GLFWwindow* getNativeHandle() const { return m_handle; }

private:
    void init();
    void centerWindow();

    // Routes GLFW C callbacks into instance methods
    static void framebufferSizeCallback(GLFWwindow* windowHandle, int width, int height);

    GLFWwindow* m_handle = nullptr;
    int m_width;
    int m_height;
    std::string m_title;
};