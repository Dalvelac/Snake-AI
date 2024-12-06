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
    void cleanup();

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

    void handleEvents();
    void update();
    void render();
    void resetGame();
    void spawnFood();
    bool checkCollision(int row, int col);
};

#endif // GAME_H
