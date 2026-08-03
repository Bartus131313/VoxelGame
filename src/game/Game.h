#pragma once

#include "../core/Window.h"
#include "entity/Player.h"
#include "render/camera/Camera3D.h"
#include "render/font/FontRenderer.h"
#include "ui/Canvas.h"
#include "ui/elements/UILabel.h"

/**
 * @brief Main application controller that manages the window lifecycle,
 * input system, and primary game loop.
 */
class Game {
public:
    static constexpr auto WINDOW_TITLE  = "Voxel Game"; ///< Title displayed in the window header.
    static constexpr int  WINDOW_WIDTH  = 1280;         ///< Initial rendering width in pixels.
    static constexpr int  WINDOW_HEIGHT = 720;          ///< Initial rendering height in pixels.

    /** @brief Constructs the Game application instance. */
    Game();

    /** @brief Destroys the Game application instance and releases resources. */
    ~Game() = default;

    // Prevent copying to maintain unique engine instance ownership
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

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

    Canvas m_canvas;                ///< UI canvas used to render some debug info. <b>Won't be here.</b>
    UILabel* m_fpsLabel = nullptr;  ///< Label that displays FPS. <b>Won't be here.</b>

    Player m_player{};                  ///< Local player functioning as a drone camera. <b>Won't be here.</b>
    Shader* m_testShader = nullptr;     ///< Test shader used to paint triangle red. <b>Won't be here.</b>
    GLuint m_testVAO{}, m_testVBO{};    ///< Test VAO and VBO used to draw the triangle. <b>Won't be here.</b>

    double m_lastUpdateTime{};    ///< Last time when game was updated.
    double m_lastRenderTime{};    ///< Last time when game was rendered.
    int m_renderedFrames{};       ///< Count of frames that were rendered since last FPS update.

    int m_fps{};          ///< Frames per second calculated every second on update pass.
};