#include "Game.h"
#include <random>
#include <iostream>
#include <algorithm> // Added for std::any_of

Game::Game() : isRunning(false), window(nullptr), renderer(nullptr), direction(1) {}

Game::~Game() = default; // Use default destructor

bool Game::init(const std::string& title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Initialization Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
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
    snake = {{10, 10}, {10, 9}, {10, 8}}; // Initial snake position
    spawnFood();
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
    // Collision check
    if (head.first < 0 || head.second < 0 || head.first >= gridRows || head.second >= gridCols || checkCollision(head.first, head.second)) {
        resetGame();
        return;
    }
    snake.insert(snake.begin(), head);

    // Food check
    if (head == food) {
        spawnFood();
    } else {
        snake.pop_back(); // Remove tail
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render the grid
    SDL_SetRenderDrawColor(renderer, 0, 100, 0, 255); // Dark green
    for (int row = 0; row < gridRows; ++row) {
        for (int col = 0; col < gridCols; ++col) {
            SDL_Rect cell = {col * cellSize, row * cellSize, cellSize, cellSize};
            SDL_RenderFillRect(renderer, &cell);
        }
    }

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

    SDL_RenderPresent(renderer);
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
}
