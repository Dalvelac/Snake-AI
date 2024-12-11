cmake_minimum_required(VERSION 3.29)
project(SnakeAI)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)

# Add include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add source files explicitly
add_executable(SnakeAI
        main.cpp          # Main entry point
        src/Game.cpp      # Game logic
        include/Game.h    # Header for game logic
)

# Define SDL_MAIN_HANDLED to prevent SDL from overriding main()
add_definitions(-DSDL_MAIN_HANDLED)

# SDL Configuration
include_directories(
        D:/SDL-release-2.30.9/build/include/SDL2 # SDL include path
        D:/SDL-release-2.30.9/include            # Backup include path
)

# Link SDL2 libraries
target_link_libraries(SnakeAI
        D:/SDL-release-2.30.9/build/libSDL2main.a  # Entry point for SDL
        D:/SDL-release-2.30.9/build/libSDL2.dll.a  # Import library for SDL2.dll
        -lmingw32                                  # MinGW compatibility library
        -luser32 -lgdi32 -lwinmm -ldxguid          # Windows-specific libraries
)

# Add a post-build step to copy the SDL2.dll to the output directory
add_custom_command(
        TARGET SnakeAI POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "D:/SDL-release-2.30.9/build/SDL2.dll"
        $<TARGET_FILE_DIR:SnakeAI>
)

# Eigen Configuration
include_directories(D:/eigen-master)              # Full path to Eigen headers

# Google Test Configuration
add_subdirectory(D:/googletest-main googletest-build)
target_link_libraries(SnakeAI gtest gtest_main)
