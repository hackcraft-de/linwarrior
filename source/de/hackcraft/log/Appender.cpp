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

