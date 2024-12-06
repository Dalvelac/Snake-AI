cmake_minimum_required(VERSION 3.29)
project(SnakeAI)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)

# Add the executable
add_executable(SnakeAI main.cpp)

# SDL Configuration
set(SDL2_DIR D:/SDL-release-2.30.9) # Update this path to your SDL directory
include_directories(${SDL2_DIR}/include)
link_directories(${SDL2_DIR}/lib)
target_link_libraries(SnakeAI SDL2 SDL2main)

# Eigen Configuration
set(EIGEN_DIR D:/eigen-master) # Update this path to your Eigen directory
include_directories(${EIGEN_DIR})

# Google Test Configuration
set(GTEST_DIR D:/googletest-main) # Update this path to your Google Test directory
add_subdirectory(${GTEST_DIR} googletest-build)
target_link_libraries(SnakeAI gtest gtest_main)

