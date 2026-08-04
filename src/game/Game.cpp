#include "Game.h"

#include <iostream>
#include <glm/glm.hpp>

#include "../core/Logger.h"
#include "../input/Input.h"
#include "glm/ext/matrix_transform.hpp"
#include "render/Renderer.h"
#include "render/texture/TextureManager.h"

Game::Game()
    : m_window(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE), m_canvas(WINDOW_WIDTH, WINDOW_HEIGHT)
{
    // Register this game as listener for all window events
    m_window.addEventListener(this);

    // Initialize global state manager
    Renderer::init();
}

void Game::update(const float deltaTime) {
    // Update Input System
    Input::update();

    // Handle game quit action
    if (Input::isActionJustPressed("game_quit")) {
        m_window.close();
        return;
    }

    // Check if window fullscreen action was pressed
    if (Input::isActionJustPressed("window_fullscreen")) {
        const WindowMode currentMode = m_window.getWindowMode();
        m_window.setWindowMode(currentMode == WindowMode::Fullscreen ? WindowMode::Windowed : WindowMode::Fullscreen);
    }

    // Toggle Cursor Lock / Release on M
    if (Input::isActionJustPressed("toggle_cursor")) {
        const bool currentLocked = Input::getMouse().isCursorLocked();
        Input::getMouse().setCursorLocked(!currentLocked);
        LOG_DEBUG("Cursor {}", !currentLocked ? "Locked" : "Unlocked");
    }

    // Update world
    m_world.update(deltaTime);
}

void Game::render() {
    // Clear background using Sky Blue color
    Renderer::clear(0.5f, 0.8f, 1.0f);

    m_world.render();

    // const auto viewMatrix = m_player.getCamera().getViewMatrix();
    // const auto projectionMatrix = m_player.getCamera().getProjectionMatrix();
    //
    // // Render procedural sky
    // m_skyRenderer.render(viewMatrix, projectionMatrix);
    //
    // // Render test triangle
    // if (m_testShader) {
    //     m_testShader->use();
    //     m_testShader->setMat4("model", m_testModelMatrix);
    //     m_testShader->setMat4("view", viewMatrix);
    //     m_testShader->setMat4("projection", projectionMatrix);
    //
    //     glBindVertexArray(m_testVAO);
    //     glDrawArrays(GL_TRIANGLES, 0, 3);
    // }

    if (m_fpsLabel) m_fpsLabel->setText("FPS: " + std::to_string(getFPS()));
    m_canvas.render();
}

void Game::onWindowResize(const int width, const int height) {
    // Change viewport using Renderer
    Renderer::setViewport(width, height);

    // Set the canvas size to the current one
    m_canvas.setSize(width, height);

    // Update local player camera aspect ratio
    if (Player* localPlayer = m_world.getLocalPlayer()) {
        localPlayer->getCamera().updateAspect(m_window.getWidth(), m_window.getHeight());
    }
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

    // Add elements to canvas
    m_fpsLabel = m_canvas.addElement<UILabel>("ProximaNova.ttf", 24, "FPS: 0");
    m_fpsLabel->setPosition(20, 20);
    m_fpsLabel->setColor(glm::vec4(0.7f, 0.0f, 1.0f, 1.0f));

    // // Setup test triangle rendering
    // m_testModelMatrix = glm::translate(m_testModelMatrix, glm::vec3(0.0f, 0.0f, -2.0f));
    // m_testShader = ShaderManager::loadShader("test");
    //
    // constexpr float vertices[] = {
    //     -0.5f, -0.5f, 0.0f,
    //      0.5f, -0.5f, 0.0f,
    //      0.0f,  0.5f, 0.0f
    // };
    //
    // glGenVertexArrays(1, &m_testVAO);
    // glGenBuffers(1, &m_testVBO);
    //
    // glBindVertexArray(m_testVAO);
    // glBindBuffer(GL_ARRAY_BUFFER, m_testVBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
    // glEnableVertexAttribArray(0);

    LOG_INFO("Game initialized.");

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
    Input::bindGamepadButton("game_quit", GLFW_GAMEPAD_BUTTON_GUIDE);

    // Create action for window fullscreen
    Input::createAction("window_fullscreen");
    Input::bindKey("window_fullscreen", GLFW_KEY_F11);
}

void Game::cleanup() {
    // Free resources from all managers
    FontManager::cleanup();
    ShaderManager::cleanup();
    TextureManager::cleanup();

    // glDeleteBuffers(1, &m_testVBO);
    // glDeleteVertexArrays(1, &m_testVAO);

    LOG_INFO("Shutting down application...");
}