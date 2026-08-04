#include "game/Game.h"
#include "core/Logger.h"

/**
 * @brief This is the entry point of the whole game.
 *
 * @return Exit code of the application.
 */
int main() {
    // Initialize Logger first so all subsequent engine systems can log safely
    Logger::init();

    Game game;
    return game.run();
}