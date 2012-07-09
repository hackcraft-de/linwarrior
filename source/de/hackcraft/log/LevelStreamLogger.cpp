#include "LevelStreamLogger.h"

#include "de/hackcraft/log/Logger.h"

#include <SDL/SDL_thread.h>


void LevelStreamLogger::appendStream() {
    
    if (logger == NULL) {
        return;
    }
    
    bool messageMode = true;
    
    unsigned int tid = SDL_ThreadID();
    
    if (!messageMode) {
        //logger->append(level, logstream.str().c_str());
        LoggerEvent* e = new LoggerEvent(logger, level,  logstream.str().c_str(), tid);
        logger->append(e);
        logstream.clear();
        logstream.str("");
    } else {
        char buffer[1024];
        logstream.clear();
        logstream.getline(buffer, sizeof(buffer), '\n');
        if (logstream.good()) {
            //logger->append(level, logstream.str().c_str());
            LoggerEvent* e = new LoggerEvent(logger, level,  logstream.str().c_str(), tid);
            //LoggerEvent* e = new LoggerEvent(logger, level, buffer);
            logger->append(e);
            logstream.str("");
        }
        logstream.clear();
    }
}
