#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <vector>
#include <utility>
#include <iostream>

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

    const int gridRows = 20;
    const int gridCols = 20;
    const int cellSize = 30;

    std::vector<std::pair<int, int>> snake; // Snake's position (row, col)
    std::pair<int, int> food;              // Food's position (row, col)
    int direction;                         // 0=Up, 1=Right, 2=Down, 3=Left
    int score;                             // Player's score
    Uint32 startTime;                      // Start time of the game

    void handleEvents();
    void update();
    void render();
    void renderHUD() const;                // Marked const for immutability
    void logDeath() const;                 // Marked const for immutability
    void resetGame();
    void spawnFood();
    void renderCheckerboard() const;       // Marked const for immutability
    void renderBorders() const;            // Marked const for immutability
    [[nodiscard]] bool checkCollision(int row, int col) const; // Added const qualifier to match the definition
};

#endif // GAME_H
