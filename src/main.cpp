#include "core/window.h"

constexpr auto WINDOW_TITLE  = "Voxel Game";
constexpr int  WINDOW_WIDTH  = 1280;
constexpr int  WINDOW_HEIGHT = 720;

int main() {
    const Window window(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);

    // Main Game Loop
    while (!window.shouldClose()) {
        window.clear(0.6f, 0.8f, 1.0f, 1.0f);

        window.update();
    }

    return 0;
}