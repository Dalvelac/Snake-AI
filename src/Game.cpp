#include "Game.h"
#include <random>
#include <iostream>
#include <fstream>
#include <algorithm> // Added for std::any_of
#include <iomanip>   // For formatting time display

Game::Game() : isRunning(false), window(nullptr), renderer(nullptr), direction(1), score(0), startTime(0) {}

Game::~Game() = default; // Use default destructor

bool Game::init(const std::string& title, int width, int height) {
    // Calculate window dimensions based on the grid size and cell size
    int windowWidth = gridCols * cellSize;
    int windowHeight = gridRows * cellSize;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Initialization Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window Creation Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer Creation Error: " << SDL_GetError() << std::endl;
        return false;
    }

    resetGame();
    isRunning = true;
    return true;
}

void Game::resetGame() {
    logDeath(); // Log score and time alive
    snake = {{10, 10}, {10, 9}, {10, 8}}; // Initial snake position
    spawnFood();
    score = 0;
    startTime = SDL_GetTicks(); // Reset timer
}

void Game::spawnFood() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distRow(0, gridRows - 1);
    std::uniform_int_distribution<> distCol(0, gridCols - 1);

    int row, col;
    do {
        row = distRow(gen);
        col = distCol(gen);
    } while (checkCollision(row, col));
    food = {row, col};
}

bool Game::checkCollision(int row, int col) const {
    return std::any_of(snake.begin(), snake.end(), [row, col](const auto& segment) {
        return segment.first == row && segment.second == col;
    });
}

void Game::handleEvents() {
    SDL_Event event{}; // Corrected initialization
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    if (direction != 2) direction = 0;
                break;
                case SDLK_RIGHT:
                    if (direction != 3) direction = 1;
                break;
                case SDLK_DOWN:
                    if (direction != 0) direction = 2;
                break;
                case SDLK_LEFT:
                    if (direction != 1) direction = 3;
                break;
                default:
                    break; // Handle default case
            }
        }
    }
}

void Game::update() {
    // Move the snake
    auto head = snake.front();
    switch (direction) {
        case 0: head.first--; break; // Up
        case 1: head.second++; break; // Right
        case 2: head.first++; break; // Down
        case 3: head.second--; break; // Left
        default: break; // Handle default case
    }
    // Check collision with borders
    if (head.first < 0 || head.second < 0 || head.first >= gridRows || head.second >= gridCols || checkCollision(head.first, head.second)) {
        resetGame(); // Reset the game on collision
        return;
    }
    snake.insert(snake.begin(), head);

    // Food check
    if (head == food) {
        spawnFood();
        score++; // Increment score when eating food
    } else {
        snake.pop_back(); // Remove tail
    }
}

void Game::render() {
    // Set the background color to dark green
    SDL_SetRenderDrawColor(renderer, 0, 100, 0, 255);
    SDL_RenderClear(renderer);

    // Render the snake
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue
    for (auto& segment : snake) {
        SDL_Rect cell = {segment.second * cellSize, segment.first * cellSize, cellSize, cellSize};
        SDL_RenderFillRect(renderer, &cell);
    }

    // Render the food
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
    SDL_Rect foodRect = {food.second * cellSize, food.first * cellSize, cellSize, cellSize};
    SDL_RenderFillRect(renderer, &foodRect);

    // Render score and time
    renderHUD();

    // Present the rendered objects
    SDL_RenderPresent(renderer);
}

void Game::renderHUD() {
    Uint32 currentTime = SDL_GetTicks();
    float timeAlive = (currentTime - startTime) / 1000.0f;

    std::cout << "Score: " << score << " | Time Alive: " << std::fixed << std::setprecision(2) << timeAlive << "s" << std::endl;
}

void Game::logDeath() {
    if (startTime == 0) return; // Skip if game hasn't started
    Uint32 endTime = SDL_GetTicks();
    float timeAlive = (endTime - startTime) / 1000.0f;

    std::ofstream logFile("game_log.txt", std::ios::app);
    logFile << "Score: " << score << ", Time Alive: " << std::fixed << std::setprecision(2) << timeAlive << "s\n";
    logFile.close();
}

void Game::cleanup() {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::run() {
    while (isRunning) {
        handleEvents();
        update();
        render();
        SDL_Delay(100); // Control game speed
    }
    logDeath(); // Log the final results before exiting
}
