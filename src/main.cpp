#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

constexpr const char* WINDOW_TITLE  = "Voxel Game";
constexpr int         WINDOW_WIDTH  = 1280;
constexpr int         WINDOW_HEIGHT = 720;

void centerWindow(GLFWwindow* window) {
    // Get window width and height
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Get monitor width and height
    int monitorWidth, monitorHeight;
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    glfwGetMonitorWorkarea(monitor, nullptr, nullptr, &monitorWidth, &monitorHeight);

    // Center the window
    glfwSetWindowPos(window, (monitorWidth - width) / 2, (monitorHeight - height) / 2);
}

int main() {
    // Check if GLFW was initialized
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!\n";
        return -1;
    }

    // Set all window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

    // Create GLFW Window
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);

    // Make the OpenGL context current on this thread before calling GL functions
    glfwMakeContextCurrent(window);

    // Initialize GLAD to load GPU function pointers
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD!\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // After initialization, center the window and make it visible
    centerWindow(window);
    glfwShowWindow(window);

    // Set clear color to BLACK
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Main Game Loop
    while (!glfwWindowShouldClose(window)) {
        // Simple input check to close on Escape key
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        glClear(GL_COLOR_BUFFER_BIT);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // Clean up resources
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}