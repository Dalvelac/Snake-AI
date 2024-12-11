#include "Game.h"
#include "Logger.h"
#include <random>
#include <algorithm> // Added for std::any_of

Game::Game() : isRunning(false), window(nullptr), renderer(nullptr), font(nullptr), direction(1), score(0), startTime(0), logger() {
    logger.startNewSession(); // Start a new logging session
}

Game::~Game() {
    if (font) {
        TTF_CloseFont(font);
    }
    TTF_Quit();
    SDL_Quit();
    logger.printGameEnd(score, startTime); // Print the game end summary
}

bool Game::init(const std::string &title, int width, int height) {
    // Calculate window dimensions based on the grid size and cell size, including borders
    int windowWidth = gridCols * cellSize + 2 * cellSize;
    int windowHeight = gridRows * cellSize + 2 * cellSize;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        logger.writeLog(Logger::LogLevel::ERROR, "SDL Initialization Error: " + std::string(SDL_GetError()));
        return false;
    }

    if (TTF_Init() == -1) {
        logger.writeLog(Logger::LogLevel::ERROR, "TTF Initialization Error: " + std::string(TTF_GetError()));
        return false;
    }

    font = TTF_OpenFont("D:/SDL-release-2.30.9/SDL2_ttf-2.22.0/Funnel_Display_Font/FunnelDisplay-VariableFont_wght.ttf", 24);
    if (!font) {
        logger.writeLog(Logger::LogLevel::ERROR, "Font Error: " + std::string(TTF_GetError()));
        return false;
    }

    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if (!window) {
        logger.writeLog(Logger::LogLevel::ERROR, "Window Creation Error: " + std::string(SDL_GetError()));
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        logger.writeLog(Logger::LogLevel::ERROR, "Renderer Creation Error: " + std::string(SDL_GetError()));
        return false;
    }

    logger.writeLog(Logger::LogLevel::INFO, "Game successfully initialized.");
    resetGame();
    isRunning = true;
    return true;
}

void Game::resetGame() {
    logger.logDeath(score, startTime); // Log death
    snake = {{10, 10}, {10, 9}, {10, 8}};
    spawnFood();
    score = 0;
    startTime = SDL_GetTicks();
    logger.writeLog(Logger::LogLevel::INFO, "Game reset.");
}

void Game::spawnFood() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distRow(1, gridRows - 2); // Prevent spawning in border rows
    std::uniform_int_distribution<> distCol(1, gridCols - 2); // Prevent spawning in border columns

    int row, col;
    do {
        row = distRow(gen);
        col = distCol(gen);
    } while (checkCollision(row, col)); // Ensure it doesn't spawn on the snake

    food = {row, col};
    logger.logEvent("Food spawned at (" + std::to_string(row) + ", " + std::to_string(col) + ")");
}

[[nodiscard]]

bool Game::checkCollision(int row, int col) const {
    // Adjust inner borders to match the visible game area
    int innerTopBorder = 1;                 // Top border index
    int innerLeftBorder = 1;                // Left border index
    int innerBottomBorder = gridRows;   // Bottom border index
    int innerRightBorder = gridCols;    // Right border index

    // Check collision with the snake's body
    bool collisionWithSnake = std::ranges::any_of(snake, [row, col](const auto &segment) {
        return segment.first == row && segment.second == col;
    });

    // Check collision with the adjusted borders
    bool collisionWithBorders = (row < innerTopBorder || col < innerLeftBorder ||
                                  row > innerBottomBorder || col > innerRightBorder);

    if (collisionWithSnake) {
        logger.logEvent("Collision with snake at (" + std::to_string(row) + ", " + std::to_string(col) + ")");
    }

    if (collisionWithBorders) {
        logger.logEvent("Collision with dark green border at (" + std::to_string(row) + ", " + std::to_string(col) + ")");
    }

    return collisionWithSnake || collisionWithBorders;
}

void Game::handleEvents() {
    SDL_Event event{};
    bool keyHandled = false;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
            return;
        }

        if (event.type == SDL_KEYDOWN && !keyHandled) {
            int newDirection = direction;
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    if (direction != 2) newDirection = 0;
                    break;
                case SDLK_RIGHT:
                    if (direction != 3) newDirection = 1;
                    break;
                case SDLK_DOWN:
                    if (direction != 0) newDirection = 2;
                    break;
                case SDLK_LEFT:
                    if (direction != 1) newDirection = 3;
                    break;
                default:
                    break;
            }
            // Only update direction if it's not reversing
            if (newDirection != (direction + 2) % 4) {
                direction = newDirection;
                keyHandled = true; // Prevent multiple key presses in one event loop
            }
        }
    }
}

void Game::update() {
    auto head = snake.front();
    switch (direction) {
        case 0: head.first--; break;
        case 1: head.second++; break;
        case 2: head.first++; break;
        case 3: head.second--; break;
        default: break;
    }

    if (checkCollision(head.first, head.second)) {
        resetGame();
        return;
    }
    snake.insert(snake.begin(), head);

    if (head == food) {
        spawnFood();
        score++;
        logger.logIntermediateScore(score);
    } else {
        snake.pop_back();
    }
}

void Game::renderCheckerboard() const {
    SDL_SetRenderDrawColor(renderer, 21, 127, 31, 255);
    SDL_RenderClear(renderer);

    for (int row = 0; row < gridRows; ++row) {
        for (int col = 0; col < gridCols; ++col) {
            bool isLight = (row + col) % 2 == 0;
            SDL_SetRenderDrawColor(renderer, isLight ? 169 : 162, isLight ? 215 : 208, isLight ? 81 : 73, 255);
            SDL_Rect cell = {col * cellSize + cellSize, row * cellSize + cellSize, cellSize, cellSize};
            SDL_RenderFillRect(renderer, &cell);
        }
    }
}

void Game::renderBorders() const {
    SDL_SetRenderDrawColor(renderer, 86, 138, 53, 255);
    SDL_Rect borders[] = {
        {0, 0, gridCols * cellSize + 2 * cellSize, cellSize},
        {0, gridRows * cellSize + cellSize, gridCols * cellSize + 2 * cellSize, cellSize},
        {0, 0, cellSize, gridRows * cellSize + 2 * cellSize},
        {gridCols * cellSize + cellSize, 0, cellSize, gridRows * cellSize + 2 * cellSize}
    };
    for (auto &border : borders) {
        SDL_RenderFillRect(renderer, &border);
    }
}

void Game::renderCircle(int x, int y, int radius, SDL_Color color) const {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // Horizontal distance from the circle's center
            int dy = radius - h; // Vertical distance from the circle's center
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

void Game::renderSnake() const {
    for (size_t i = 0; i < snake.size(); ++i) {
        float t = static_cast<float>(i) / (snake.size() - 1); // Gradient factor
        auto r = static_cast<Uint8>(68 + t * (77 - 68));
        auto g = static_cast<Uint8>(110 + t * (123 - 110));
        auto b = static_cast<Uint8>(231 + t * (242 - 231));

        const auto &segment = snake[i];
        int x = segment.second * cellSize + cellSize / 4; // Adjust for rounded edges
        int y = segment.first * cellSize + cellSize / 4;
        int size = cellSize / 2;

        SDL_Color segmentColor = {r, g, b, 255}; // Add the color for the segment
        renderCircle(x + size / 2, y + size / 2, size, segmentColor);
    }
}


void Game::renderApple() const {
    int x = food.second * cellSize + cellSize / 2;
    int y = food.first * cellSize + cellSize / 2;

    SDL_Color appleColor = {230, 71, 29, 255}; // Add the color for the apple
    renderCircle(x, y, cellSize / 2, appleColor);

    // Render the stem
    SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255); // Green color for the stem
    SDL_Rect stem = {x - cellSize / 8, y - cellSize / 2, cellSize / 4, cellSize / 8};
    SDL_RenderFillRect(renderer, &stem);
}


void Game::render() {
    renderCheckerboard();
    renderBorders();
    renderSnake();
    renderApple();
    renderHUD();
    SDL_RenderPresent(renderer);

    logger.trackFPS();
}

void Game::renderHUD() const {
    SDL_Color color = {255, 255, 255, 255};
    Uint32 currentTime = SDL_GetTicks();
    float timeAlive = static_cast<float>(currentTime - startTime) / 1000.0f;

    // Score and Time Counter (Top-right corner)
    std::string hudText = "Score: " + std::to_string(score) + " | Time: " + std::to_string(static_cast<int>(timeAlive)) + "s";
    SDL_Surface *surface = TTF_RenderText_Solid(font, hudText.c_str(), color);
    if (surface) {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect textRect = {gridCols * cellSize - surface->w - 0, 0, surface->w, surface->h}; // Reduced padding to move text closer to the right edge
        SDL_RenderCopy(renderer, texture, nullptr, &textRect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    // "Snake AI" Title (Top-left of the window)
    std::string titleText = "Snake AI";
    surface = TTF_RenderText_Solid(font, titleText.c_str(), color);
    if (surface) {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect textRect = {10, 0, surface->w, surface->h}; // Adjusted to top-left corner
        SDL_RenderCopy(renderer, texture, nullptr, &textRect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
}

void Game::cleanup() const {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
}

void Game::run() {
    constexpr int targetFPS = 60;
    constexpr int frameDelay = 1000 / targetFPS; // Frame delay for rendering
    constexpr int logicDelay = 100; // Game logic updates every 100ms (adjust to match current snake speed)

    Uint32 lastLogicTime = SDL_GetTicks();
    Uint32 lastFrameTime = SDL_GetTicks();

    while (isRunning) {
        Uint32 currentTime = SDL_GetTicks();
        Uint32 elapsedLogicTime = currentTime - lastLogicTime;
        Uint32 elapsedFrameTime = currentTime - lastFrameTime;

        // Handle logic updates
        if (elapsedLogicTime >= logicDelay) {
            handleEvents();
            update();
            lastLogicTime += logicDelay; // Prevent logic from running too many times
        }

        // Render more frequently (smooth rendering)
        if (elapsedFrameTime >= frameDelay) {
            render();
            lastFrameTime = currentTime;
        }

        SDL_Delay(1); // Small delay to prevent high CPU usage
    }

    logger.logDeath(score, startTime);
    logger.logMaxScore(score);
}
