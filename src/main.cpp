#include "core/Window.h"
#include "core/input/Input.h"

#include <iostream>

constexpr auto WINDOW_TITLE  = "Voxel Game";
constexpr int  WINDOW_WIDTH  = 1280;
constexpr int  WINDOW_HEIGHT = 720;

void setupInputBindings() {
    Input::createAction("toggle_cursor");
    Input::bindKey("toggle_cursor", GLFW_KEY_M);
    Input::bindGamepadButton("toggle_cursor", GLFW_GAMEPAD_BUTTON_START);

    Input::createAction("game_quit");
    Input::bindKey("game_quit", GLFW_KEY_ESCAPE);
    Input::bindGamepadButton("toggle_cursor", GLFW_GAMEPAD_BUTTON_GUIDE);
}

int main() {
    // Create Window
    const Window window(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);

    // Initialize Input System & Register Action Mappings
    Input::init(window.getNativeHandle());
    setupInputBindings();

    // Lock cursor by default for FPS
    Input::getMouse().setCursorLocked(true);

    std::cout << "[Game] Initialized successfully!\n";

    // Main game loop
    while (!window.shouldClose()) {
        // Update input system at the start of the frame
        Input::update();

        // Handle game quit action
        if (Input::isActionJustPressed("game_quit")) {
            window.close();
            break; // Break cuz window was closed so no need to update and render
        }

        // Toggle Cursor Lock / Release on M
        if (Input::isActionJustPressed("toggle_cursor")) {
            const bool currentLocked = Input::getMouse().isCursorLocked();
            Input::getMouse().setCursorLocked(!currentLocked);
            std::cout << "[Game] Cursor " << (!currentLocked ? "Locked" : "Unlocked") << std::endl;
        }

        // Check Mouse Scroll
        if (const glm::vec2 scrollDelta = Input::getMouse().getScrollDelta(); scrollDelta.y != 0.0f) {
            std::cout << "[Game] Mouse Scroll Y: " << scrollDelta.y << "\n";
        }

        // Render pass
        window.clear(0.6f, 0.8f, 1.0f, 1.0f);

        // Swap buffer
        window.update();
    }

    return 0;
}