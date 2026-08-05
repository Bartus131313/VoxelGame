#pragma once

#include "../core/Window.h"
#include "entity/impl/Player.h"
#include "render/mesh/impl/SkyboxMesh.h"
#include "render/sky/SkyRenderer.h"
#include "ui/Canvas.h"
#include "ui/elements/UILabel.h"
#include "world/World.h"

/**
 * @brief Main application controller that manages the window lifecycle,
 * input system, and primary game loop.
 */
class Game : public IWindowEventListener {
public:
    // Window configuration

    static constexpr auto WINDOW_TITLE  = "Voxel Game"; ///< Title displayed in the window header.
    static constexpr int  WINDOW_WIDTH  = 1280;         ///< Initial rendering width in pixels.
    static constexpr int  WINDOW_HEIGHT = 720;          ///< Initial rendering height in pixels.

    // -----

    // Game configuration

    static constexpr int  MAX_RENDER_DISTANCE   = 32;    /// Max amount of chunks in any direction that can be rendered.

    static constexpr float TICKS_PER_SECOND = 20.0f;    ///< How many ticks are in 1 second?

    // -----

    /** @brief Constructs the Game application instance. */
    Game();

    /** @brief Destroys the Game application instance and releases resources. */
    ~Game() override = default;

    // Prevent copying to maintain unique engine instance ownership
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    /**
     * @brief Called by Window class when framebuffer size was changed.
     *
     * @param width Width of the window.
     * @param height Height of the window.
     */
    void onWindowResize(int width, int height) override;

    /**
     * @brief Gets calculated FPS.
     *
     * @return Frames per second, if game was not run yet, returns 0.
     */
    [[nodiscard]] int getFPS() const { return m_fps; }

    /**
     * @brief Starts the main application loop, handling initialization, updates, and rendering.
     *
     * @return Application exit code.
     */
    int run();
private:
    /**
     * @brief Registers logical action mappings and binds them to default keyboard,
     * mouse, and gamepad inputs.
     */
    static void setupInputBindings();

    /**
     * @brief Called on update pass. Updates the whole game with given delta time.
     *
     * @param deltaTime Time passed between previous and current frame.
     */
    void update(float deltaTime);

    /** @brief Called on render pass. Renders the whole game. */
    void render();

    /** @brief Performs cleanup operations when the game loop terminates. */
    void cleanup();

    Window m_window;    ///< Core window wrapper managing the GLFW context and render surface.

    Canvas m_canvas;                                    ///< Debug UI canvas used to render some debug info.
    std::shared_ptr<UILabel> m_fpsLabel = nullptr;      ///< Debug label that displays FPS.

    World m_world;

    double m_lastUpdateTime{};      ///< Last time when game was updated.
    double m_lastRenderTime{};      ///< Last time when game was rendered.
    int m_renderedFrames{};         ///< Count of frames that were rendered since last FPS update.

    int m_fps{};          ///< Frames per second calculated every second on update pass.
};