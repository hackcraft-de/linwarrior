#include "Appender.h"

#include "de/hackcraft/log/Logger.h"

#include <iostream>

Appender::Appender() {
}


Appender::~Appender() {
}


void Appender::append(LoggerEvent* loggerEvent) {
    
    int level = loggerEvent->getLevel();
    //int tid = loggerEvent->getTid();
    
    // Don't print any debug or trace messages for the time being.
    if (level > 2) {
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

