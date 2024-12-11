#ifndef GAME_H
#define GAME_H

#include "Logger.h" // Include Logger
#include <SDL.h>
#include <vector>
#include <utility>
#include <string> // For std::string
#include <..\include\SDL2\SDL_ttf.h>

class Game {
public:
    Game();
    ~Game();

    bool init(const std::string& title, int width, int height);
    void run();
    void cleanup() const;

private:
    bool isRunning;
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font; // Font for rendering HUD

    const int gridRows = 20;
    const int gridCols = 20;
    const int cellSize = 30;

    std::vector<std::pair<int, int>> snake; // Snake's position (row, col)
    std::pair<int, int> food;              // Food's position (row, col)
    int direction;                         // 0=Up, 1=Right, 2=Down, 3=Left
    int score;                             // Player's score
    Uint32 startTime;                      // Start time of the game

    Logger logger;                         // Logger instance for managing logs

    void handleEvents();
    void update();
    void render();
    void renderHUD() const;
    void resetGame();
    void spawnFood();
    void renderCheckerboard() const;
    void renderBorders() const;
    [[nodiscard]] bool checkCollision(int row, int col) const;
};

#endif // GAME_H
