#include "Logger.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <filesystem>
#include <ctime>

Logger::Logger() : testRun{1}, sessionId{1}, logLevel{INFO}, fpsFrameCount{0}, fpsStartTime{SDL_GetTicks()} {}

void Logger::startNewSession() {
    sessionStartTime = std::chrono::system_clock::now();
    std::string timestamp = getTimestamp(sessionStartTime);
    std::string sessionFileName = "Session_" + std::to_string(sessionId++) + "_" + timestamp + ".txt";
    sessionLogFile.open(sessionFileName, std::ios::out);
    if (sessionLogFile.is_open()) {
        sessionLogFile << "Session started at: " << timestamp << "\n";
    }
}

void Logger::logDeath(int score, Uint32 startTime) {
    if (startTime == 0) return;
    Uint32 endTime = SDL_GetTicks();
    float timeAlive = static_cast<float>(endTime - startTime) / 1000.0f;

    std::string message = "Score: " + std::to_string(score) + ", Time Alive: " + std::to_string(timeAlive) + "s\n";
    writeLog(INFO, message);

    if (sessionLogFile.is_open()) {
        sessionLogFile << message;
    }
}

void Logger::logMaxScore(int score) {
    std::string fileName = "MaxScore_Testrun" + std::to_string(testRun++) + ".txt";
    std::ofstream outFile(fileName);
    if (outFile.is_open()) {
        outFile << "Max Score: " << score << "\n";
        outFile.close();
    } else {
        writeLog(ERROR, "Failed to write max score log: " + fileName);
    }
}

void Logger::logIntermediateScore(int score) {
    std::string message = "Current Score: " + std::to_string(score) + "\n";
    writeLog(INFO, message);

    if (sessionLogFile.is_open()) {
        sessionLogFile << message;
    }
}

void Logger::logEvent(const std::string &event) const {
    std::string message = "Event: " + event + "\n";
    if (sessionLogFile.is_open()) {
        sessionLogFile << message;
    }
}

void Logger::trackFPS() {
    fpsFrameCount++;
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - fpsStartTime >= 1000) { // Every second
        float fps = static_cast<float>(fpsFrameCount) / ((currentTime - fpsStartTime) / 1000.0f);
        fpsStartTime = currentTime;
        fpsFrameCount = 0;

        std::string message = "FPS: " + std::to_string(fps) + "\n";
        writeLog(INFO, message);

        if (sessionLogFile.is_open()) {
            sessionLogFile << message;
        }
    }
}

void Logger::logAchievement(const std::string &achievement) {
    std::string message = "Achievement: " + achievement + "\n";
    writeLog(INFO, message);

    if (sessionLogFile.is_open()) {
        sessionLogFile << message;
    }
}

void Logger::logPlayerBehavior(const std::string &behavior) {
    std::string message = "Player Behavior: " + behavior + "\n";
    writeLog(DEBUG, message);

    if (sessionLogFile.is_open()) {
        sessionLogFile << message;
    }
}

void Logger::writeLog(LogLevel level, const std::string &message) const {
    if (level < logLevel) return; // Only log messages of the set level or higher
    std::cout << "[" << logLevelToString(level) << "] " << message;

    if (sessionLogFile.is_open()) {
        sessionLogFile << "[" << logLevelToString(level) << "] " << message;
    }
}

void Logger::printGameEnd(int score, Uint32 startTime) {
    Uint32 endTime = SDL_GetTicks();
    float timeAlive = static_cast<float>(endTime - startTime) / 1000.0f;

    std::string message = "Game Ended! Final Score: " + std::to_string(score) + ", Time: " + std::to_string(timeAlive) + "s\n";
    writeLog(INFO, message);

    if (sessionLogFile.is_open()) {
        sessionLogFile << message;
        sessionLogFile.close();
    }
}

std::string Logger::getTimestamp(const std::chrono::system_clock::time_point &timePoint) {
    std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S", std::localtime(&time));
    return std::string(buffer);
}

std::string Logger::logLevelToString(LogLevel level) {
    switch (level) {
        case INFO:
            return "INFO";
        case DEBUG:
            return "DEBUG";
        case WARN:
            return "WARN";
        case ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}
