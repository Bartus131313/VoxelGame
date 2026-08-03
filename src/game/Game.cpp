#include "Game.h"

#include <iostream>
#include <glm/glm.hpp>

#include "../input/Input.h"
#include "glm/ext/matrix_transform.hpp"

Game::Game()
    : m_window(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE), m_canvas(WINDOW_WIDTH, WINDOW_HEIGHT)
{
    // Register this game as listener for all window events
    m_window.addEventListener(this);
}

void Game::update(const float deltaTime) {
    // Update Input System
    Input::update();

    // Handle game quit action
    if (Input::isActionJustPressed("game_quit")) {
        glfwSetWindowShouldClose(m_window.getNativeHandle(), GLFW_TRUE);
        return;
    }

    // Toggle Cursor Lock / Release on M
    if (Input::isActionJustPressed("toggle_cursor")) {
        const bool currentLocked = Input::getMouse().isCursorLocked();
        Input::getMouse().setCursorLocked(!currentLocked);
        std::cout << "[Game] Cursor " << (!currentLocked ? "Locked" : "Unlocked") << std::endl;
    }

    // Check Mouse Scroll - DEBUG ONLY
    if (const glm::vec2 scrollDelta = Input::getMouse().getScrollDelta(); scrollDelta.y != 0.0f) {
        std::cout << "[Game] Mouse Scroll Y: " << scrollDelta.y << "\n";
    }

    // Update player
    m_player.update(deltaTime);
}

void Game::render() {
    // Clear background using Sky Blue color
    m_window.clear(0.6f, 0.8f, 1.0f, 1.0f);

    // Render test triangle
    if (m_testShader) {
        m_testShader->use();
        m_testShader->setMat4("model", m_testModelMatrix);
        m_testShader->setMat4("view", m_player.getCamera().getViewMatrix());
        m_testShader->setMat4("projection", m_player.getCamera().getProjectionMatrix());

        glBindVertexArray(m_testVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    if (m_fpsLabel) m_fpsLabel->setText("FPS: " + std::to_string(getFPS()));
    m_canvas.render();
}

void Game::onWindowResize(const int width, const int height) {
    // Set the canvas size to the current one
    m_canvas.setSize(width, height);

    // Update player camera aspect ratio
    m_player.getCamera().updateAspect(m_window.getWidth(), m_window.getHeight());
}

int Game::run() {
    // Initialize Input System
    Input::init(m_window.getNativeHandle());

    // Register Action Mappings
    setupInputBindings();

    // Lock cursor by default for FPS camera tracking
    Input::getMouse().setCursorLocked(true);

    // Set last update and render time to current time
    double currentTime = glfwGetTime();
    m_lastUpdateTime = currentTime;
    m_lastRenderTime = currentTime;

    std::cout << "[Game] Initialized successfully!\n";

    // Add elements to canvas
    m_fpsLabel = m_canvas.addElement<UILabel>("ProximaNova.ttf", 24, "FPS: 0");
    m_fpsLabel->setPosition(20, 20);
    m_fpsLabel->setColor(glm::vec4(0.7f, 0.0f, 1.0f, 1.0f));

    // Setup test triangle rendering
    m_testModelMatrix = glm::translate(m_testModelMatrix, glm::vec3(0.0f, 0.0f, -2.0f));
    m_testShader = ShaderManager::loadShader("test");

    constexpr float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    glGenVertexArrays(1, &m_testVAO);
    glGenBuffers(1, &m_testVBO);

    glBindVertexArray(m_testVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_testVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);

    // Main game loop
    while (!m_window.shouldClose()) {
        // Update pass
        currentTime = glfwGetTime();
        const double deltaTime = currentTime - m_lastUpdateTime;
        m_lastUpdateTime = currentTime;
        update(static_cast<float>(deltaTime));

        // Render pass
        if (currentTime - m_lastRenderTime > 1.0) {
            m_lastRenderTime = currentTime;
            m_fps = m_renderedFrames;
            m_renderedFrames = 0;
        }
        render();

        // Swap buffers
        m_window.update();
        m_renderedFrames++;
    }

    cleanup();
    return 0;
}

void Game::setupInputBindings() {
    // Create action for cursor lock toggle
    Input::createAction("toggle_cursor");
    Input::bindKey("toggle_cursor", GLFW_KEY_M);
    Input::bindGamepadButton("toggle_cursor", GLFW_GAMEPAD_BUTTON_START);

    // Create action for game quit
    Input::createAction("game_quit");
    Input::bindKey("game_quit", GLFW_KEY_ESCAPE);
    Input::bindGamepadButton("game_quit", GLFW_GAMEPAD_BUTTON_GUIDE); // Fixed typo from earlier snippet
}

void Game::cleanup() {
    FontManager::cleanup();
    ShaderManager::cleanup();
    std::cout << "[Game] Shutting down application...\n";
}