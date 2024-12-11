#include "Logger.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <filesystem>


Logger::Logger() : testRun(1) {}

void Logger::logDeath(int score, Uint32 startTime) {
    if (startTime == 0) return; // Skip if game hasn't started
    Uint32 endTime = SDL_GetTicks();
    float timeAlive = static_cast<float>(endTime - startTime) / 1000.0f;

    std::ofstream logFile("game_log.txt", std::ios::app);
    logFile << "Score: " << score << ", Time Alive: " << std::fixed << std::setprecision(2) << timeAlive << "s\n";
    logFile.close();
}

void Logger::logMaxScore(int score) {
    std::string fileName = "MaxScore_Testrun" + std::to_string(testRun++) + ".txt";
    std::ofstream outFile(fileName);
    if (outFile.is_open()) {
        outFile << "Max Score: " << score << "\n";
        outFile.close();
    } else {
        std::cerr << "Error writing to " << fileName << "\n";
    }
}

void Logger::printGameEnd(int score, Uint32 startTime) {
    Uint32 endTime = SDL_GetTicks();
    float timeAlive = static_cast<float>(endTime - startTime) / 1000.0f;
    std::cout << "Game Ended! Final Score: " << score << ", Time: " << timeAlive << "s\n";
}
