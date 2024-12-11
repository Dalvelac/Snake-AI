#ifndef LOGGER_H
#define LOGGER_H

#include <SDL.h>
#include <string>
#include <fstream>
#include <chrono>

class Logger {
public:
    enum LogLevel {
        INFO,
        DEBUG,
        WARN,
        ERROR
    };

    Logger();

    // Session management
    void startNewSession();

    // Logging functions
    void logDeath(int score, Uint32 startTime);
    void logMaxScore(int score);
    void logIntermediateScore(int score);
    void logEvent(const std::string &event) const;
    void trackFPS();
    void logAchievement(const std::string &achievement);
    void logPlayerBehavior(const std::string &behavior);
    void printGameEnd(int score, Uint32 startTime);

    // Utility functions
    void writeLog(LogLevel level, const std::string &message) const;
    std::string getTimestamp(const std::chrono::system_clock::time_point &timePoint) const;
    std::string logLevelToString(LogLevel level) const;

private:
    int testRun;                             // Test run identifier
    int sessionId;                           // Session ID
    LogLevel logLevel;                       // Current logging level
    mutable std::ofstream sessionLogFile;   // Log file stream for the session
    std::chrono::system_clock::time_point sessionStartTime;

    // FPS tracking
    int fpsFrameCount;
    Uint32 fpsStartTime;
};

#endif // LOGGER_H
