#include "Game.h"

#include <iostream>
#include <glm/glm.hpp>
#include "../input/Input.h"
#include "glm/ext/matrix_clip_space.hpp"

Game::Game()
    : m_window(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE), m_canvas(WINDOW_WIDTH, WINDOW_HEIGHT), m_fps(0) {
}

void Game::update(float deltaTime) {
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

    // Set the canvas size to current one
    m_canvas.setSize(m_window.getWidth(), m_window.getHeight());
}

void Game::render() {
    // Clear background using Sky Blue color
    m_window.clear(0.6f, 0.8f, 1.0f, 1.0f);

    if (m_fpsLabel) m_fpsLabel->setText("FPS: " + std::to_string(getFPS()));
    m_canvas.render();
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
    m_fpsLabel = m_canvas.addElement<UILabel>("ProximaNova.ttf", 24, "FPS: 0").get();
    m_fpsLabel->setPosition(20, 20);
    m_fpsLabel->setColor(glm::vec4(0.7f, 0.0f, 1.0f, 1.0f));

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
    std::cout << "[Game] Shutting down application...\n";
}