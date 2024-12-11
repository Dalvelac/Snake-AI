cmake_minimum_required(VERSION 3.29)
project(SnakeAI)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_VERBOSE_MAKEFILE ON)

# Add include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add source files explicitly
add_executable(SnakeAI
        main.cpp          # Main entry point
        src/Game.cpp      # Game logic
        include/Game.h
        src/Logger.cpp
        include/Logger.h    # Header for game logic
)

# Define SDL_MAIN_HANDLED to prevent SDL from overriding main()
add_definitions(-DSDL_MAIN_HANDLED)
add_definitions(-DTTF_EXPORTS)

# SDL Configuration
include_directories(
        D:/SDL-release-2.30.9/build/include/SDL2       # SDL include path
        D:/SDL-release-2.30.9/include                  # Additional SDL include path
        D:/SDL-release-2.30.9/SDL2_ttf-2.22.0         # SDL2_ttf root directory
        D:/SDL-release-2.30.9/SDL2_ttf-2.22.0/x86_64-w64-mingw32/include # SDL2_ttf include path
)

# Link SDL2 and SDL2_ttf libraries
target_link_libraries(SnakeAI
        D:/SDL-release-2.30.9/build/libSDL2main.a     # SDL2 main library
        D:/SDL-release-2.30.9/build/libSDL2.dll.a     # SDL2 import library
        D:/SDL-release-2.30.9/SDL2_ttf-2.22.0/x86_64-w64-mingw32/lib/libSDL2_ttf.dll.a # SDL2_ttf library
        -lmingw32                                     # MinGW compatibility library
        -luser32 -lgdi32 -lwinmm -ldxguid             # Windows-specific libraries
)

# Post-build steps to copy necessary DLLs to the output directory
add_custom_command(
        TARGET SnakeAI POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "D:/SDL-release-2.30.9/build/SDL2.dll"
        $<TARGET_FILE_DIR:SnakeAI>
)
add_custom_command(
        TARGET SnakeAI POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "D:/SDL-release-2.30.9/SDL2_ttf-2.22.0/x86_64-w64-mingw32/bin/SDL2_ttf.dll"
        $<TARGET_FILE_DIR:SnakeAI>
)

# Eigen Configuration
include_directories(D:/eigen-master)              # Full path to Eigen headers

# Google Test Configuration
add_subdirectory(D:/googletest-main googletest-build)
target_link_libraries(SnakeAI gtest gtest_main)
