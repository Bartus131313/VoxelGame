#include "Game.h"

#include <iostream>
#include <glm/glm.hpp>

#include "../core/Logger.h"
#include "../input/Input.h"
#include "glm/ext/matrix_transform.hpp"
#include "render/RenderSystem.h"
#include "render/texture/TextureManager.h"

Game::Game()
    : m_window(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE), m_canvas(WINDOW_WIDTH, WINDOW_HEIGHT)
{
    // Register this game as listener for all window events
    m_window.addEventListener(this);

    // Initialize global state manager
    RenderSystem::init();
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

    // Toggle Render Mode (Fill/Line)
    if (Input::isActionJustPressed("toggle_render_mode")) {
        const RenderMode currentMode = RenderSystem::getRenderMode();
        RenderSystem::setRenderMode(currentMode == Fill ? Line : Fill);
    }

    // Update world
    m_world.update(deltaTime);
}

void Game::render() {
    // Clear background using Sky Blue color
    RenderSystem::clear(0.5f, 0.8f, 1.0f);

    // Render world
    m_world.render();

    // Update all dynamic labels
    if (m_fpsLabel) m_fpsLabel->setText("FPS: " + std::to_string(getFPS()));
    if (m_renderModeLabel) m_renderModeLabel->setText(std::format("Render mode: {}", RenderSystem::getRenderMode()));

    // Render UI canvas
    m_canvas.render();
}

void Game::onWindowResize(const int width, const int height) {
    // Change viewport using Renderer
    RenderSystem::setViewport(width, height);

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
    ResourceLocation uiFontLocation{"fonts/ProximaNova.ttf"};
    m_fpsLabel = m_canvas.addElement<UILabel>(uiFontLocation, 32, "FPS: 0");
    if (m_fpsLabel) {
        m_fpsLabel->setPosition(10, 10);
        m_fpsLabel->setColor(glm::vec4(0.7f, 0.0f, 1.0f, 1.0f));
    }

    m_renderModeLabel = m_canvas.addElement<UILabel>(uiFontLocation, 24, "Render mode: FILL");
    if (m_renderModeLabel) {
        m_renderModeLabel->setPosition(10, 42);
        m_renderModeLabel->setColor(glm::vec4(0.5f, 0.0f, 0.8f, 1.0f));
    }

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

    // Create action for render mode toggle
    Input::createAction("toggle_render_mode");
    Input::bindKey("toggle_render_mode", GLFW_KEY_P);
    Input::bindGamepadButton("toggle_render_mode", GLFW_GAMEPAD_BUTTON_Y);
}

void Game::cleanup() {
    // Free resources from all managers
    FontManager::cleanup();
    ShaderManager::cleanup();
    TextureManager::cleanup();

    LOG_INFO("Shutting down application...");
}