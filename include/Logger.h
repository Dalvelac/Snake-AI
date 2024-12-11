#ifndef LOGGER_H
#define LOGGER_H

#include <SDL.h>
class Logger {
public:
    Logger();

    // Logs the death details of the game
    static void logDeath(int score, Uint32 startTime) ;

    // Logs the maximum score for the current test run
    void logMaxScore(int score);

    // Prints the game end details to the console
    static void printGameEnd(int score, Uint32 startTime) ;

private:
    int testRun; // Keeps track of the test run number
};

#endif // LOGGER_H
