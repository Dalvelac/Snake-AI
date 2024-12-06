#include <SDL.h>
#include <iostream>
#include "Game.h"

int main() {
    Game game;

    if (!game.init("SnakeAI", 800, 600)) {
        std::cerr << "Failed to initialize game." << std::endl;
        return -1;
    }

    game.run();
    game.cleanup();

    return 0;
}
