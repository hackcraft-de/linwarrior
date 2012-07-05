/* 
 * File:    LoggerEvent.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on July 5, 2012, 8:12 PM
 */


#ifndef LOGGEREVENT_H
#define	LOGGEREVENT_H

class LoggerEvent;

#include "de/hackcraft/log/Logger.h"

#include <string>

class LoggerEvent {
public:
    LoggerEvent(Logger* logger, int level, const char* message, unsigned int tid);
    
    virtual ~LoggerEvent();
    
    Logger* getLogger();
    int getLevel();
    const char* getMessage();
    unsigned int getTid();
    
private:
    Logger* logger;
    int level;
    std::string message;
    unsigned int tid;
};

#endif	/* LOGGEREVENT_H */

