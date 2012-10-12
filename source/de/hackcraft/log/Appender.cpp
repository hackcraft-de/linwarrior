#include "Appender.h"

#include "de/hackcraft/log/Logger.h"

#include <iostream>

Appender::Appender() {
    maxLevel = 2;
}


Appender::~Appender() {
}


void Appender::append(LoggerEvent* loggerEvent) {
    
    int level = loggerEvent->getLevel();
    //int tid = loggerEvent->getTid();
    
    // Don't print any debug or trace messages for the time being.
    if (level > maxLevel) {
        return;
    }
    
    // Output log message to standard output stream (or redirection thereof).
    std::cout 
            << clock() / 1000
            << " "
            << Logger::getLevelName(level) 
            << " "
            //<< "[" << tid << "] "
            << loggerEvent->getLogger()->getName()
            << " - "
            << loggerEvent->getMessage();
}


void Appender::setMaxLevel(int maxLevel) {
    this->maxLevel = maxLevel;
}


int Appender::getMaxLevel() {
    return maxLevel;
}

