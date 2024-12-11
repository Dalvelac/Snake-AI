#include "Game.h"
#include "Logger.h"
#include <random>
#include <iostream>
#include <algorithm> // Added for std::any_of
#include <iomanip>// For formatting time display

Game::Game() : isRunning(false), window(nullptr), renderer(nullptr), font(nullptr), direction(1), score(0), startTime(0), logger() {}

Game::~Game() {
    if (font) {
        TTF_CloseFont(font);
    }
    TTF_Quit();
    SDL_Quit();
}

bool Game::init(const std::string& title, int width, int height) {
    // Calculate window dimensions based on the grid size and cell size, including borders
    int windowWidth = gridCols * cellSize + 2 * cellSize; // Add space for borders
    int windowHeight = gridRows * cellSize + 2 * cellSize; // Add space for borders

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Initialization Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() == -1) {
        std::cerr << "TTF Initialization Error: " << TTF_GetError() << std::endl;
        return false;
    }

    font = TTF_OpenFont("D:/SDL-release-2.30.9/SDL2_ttf-2.22.0/Funnel_Display_Font/FunnelDisplay-VariableFont_wght.ttf", 24);
    if (!font) {
        std::cerr << "Font Error: " << TTF_GetError() << std::endl;
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
    Logger::logDeath(score, startTime); // Log death when the game resets
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
    return std::ranges::any_of(snake, [row, col](const auto& segment) {
        return segment.first == row && segment.second == col;
    }) || row < 0 || col < 0 || row >= gridRows || col >= gridCols; // Include border collision
}

void Game::handleEvents() {
    SDL_Event event{}; // Corrected initialization
    static int lastDirection = -1;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }
        if (event.type == SDL_KEYDOWN) {
            if ((event.key.keysym.mod & KMOD_ALT) && (event.key.keysym.mod & KMOD_SHIFT) && event.key.keysym.sym == SDLK_z) {
                isRunning = false;
                logger.logMaxScore(score); // Log max score when exiting
                Logger::printGameEnd(score, startTime); // Print final stats
                return;
            }
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    if (direction != 2 && lastDirection != 2) direction = 0;
                break;
                case SDLK_RIGHT:
                    if (direction != 3 && lastDirection != 3) direction = 1;
                break;
                case SDLK_DOWN:
                    if (direction != 0 && lastDirection != 0) direction = 2;
                break;
                case SDLK_LEFT:
                    if (direction != 1 && lastDirection != 1) direction = 3;
                break;
                default:
                    break; // Handle default case
            }
            lastDirection = direction;
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
    if (checkCollision(head.first, head.second)) {
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

void Game::renderCheckerboard() const {
    SDL_SetRenderDrawColor(renderer, 21, 127, 31, 255); // Dark green background
    SDL_RenderClear(renderer);

    for (int row = 0; row < gridRows; ++row) {
        for (int col = 0; col < gridCols; ++col) {
            bool isLight = (row + col) % 2 == 0;
            SDL_SetRenderDrawColor(renderer, isLight ? 76 : 21, isLight ? 185 : 127, isLight ? 99 : 31, 255); // Alternating light/dark green
            SDL_Rect cell = {col * cellSize + cellSize, row * cellSize + cellSize, cellSize, cellSize};
            SDL_RenderFillRect(renderer, &cell);
        }
    }
}

void Game::renderBorders() const {
    SDL_SetRenderDrawColor(renderer, 29, 38, 59, 255); // Border color

    // Define border rectangles
    SDL_Rect topBorder = {0, 0, gridCols * cellSize + 2 * cellSize, cellSize};                    // Top border
    SDL_Rect bottomBorder = {0, gridRows * cellSize + cellSize, gridCols * cellSize + 2 * cellSize, cellSize}; // Bottom border
    SDL_Rect leftBorder = {0, 0, cellSize, gridRows * cellSize + 2 * cellSize};                   // Left border
    SDL_Rect rightBorder = {gridCols * cellSize + cellSize, 0, cellSize, gridRows * cellSize + 2 * cellSize};  // Right border

    // Render all border rectangles
    SDL_RenderFillRect(renderer, &topBorder);
    SDL_RenderFillRect(renderer, &bottomBorder);
    SDL_RenderFillRect(renderer, &leftBorder);
    SDL_RenderFillRect(renderer, &rightBorder);
}

void Game::render() {
    // Render checkerboard background
    renderCheckerboard();

    // Render borders
    renderBorders();

    // Render the snake
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue
    for (const auto& segment : snake) {
        SDL_Rect cell = {segment.second * cellSize + cellSize, segment.first * cellSize + cellSize, cellSize, cellSize};
        SDL_RenderFillRect(renderer, &cell);
    }

    // Render the food
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
    SDL_Rect foodRect = {food.second * cellSize + cellSize, food.first * cellSize + cellSize, cellSize, cellSize};
    SDL_RenderFillRect(renderer, &foodRect);

    // Render score and time
    renderHUD();

    // Present the rendered objects
    SDL_RenderPresent(renderer);
}

void Game::renderHUD() const {
    SDL_Color color = {255, 255, 255, 255}; // White color
    Uint32 currentTime = SDL_GetTicks();
    float timeAlive = static_cast<float>(currentTime - startTime) / 1000.0f;

    std::string hudText = "Score: " + std::to_string(score) + " | Time: " + std::to_string(static_cast<int>(timeAlive)) + "s";

    SDL_Surface* surface = TTF_RenderText_Solid(font, hudText.c_str(), color);
    if (!surface) {
        std::cerr << "Text Rendering Error: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        std::cerr << "Texture Creation Error: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_Rect textRect = {10, 10, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &textRect);
    SDL_DestroyTexture(texture);
}

void Game::cleanup() const {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
}

void Game::run() {
    while (isRunning) {
        handleEvents();
        update();
        render();
        SDL_Delay(100); // Control game speed
    }
    Logger::logDeath(score, startTime); // Log death when the game ends
    logger.logMaxScore(score);        // Log max score
}
