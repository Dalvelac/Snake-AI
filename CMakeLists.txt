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

# SDL Configuration
include_directories(D:/SDL-release-2.30.9/build/include/SDL2) # Full path to SDL headers
target_link_libraries(SnakeAI
        D:/SDL-release-2.30.9/libSDL2.a       # Full path to SDL2 library
        D:/SDL-release-2.30.9/libSDL2main.a   # Full path to SDL2main library
)

# Eigen Configuration
include_directories(D:/eigen-master) # Full path to Eigen headers

# Google Test Configuration
add_subdirectory(D:/googletest-main googletest-build)
target_link_libraries(SnakeAI gtest gtest_main)
