/* 
 * File:    LevelStreamLogger.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on July 3, 2012, 8:48 PM
 */


#ifndef LEVELSTREAMLOGGER_H
#define	LEVELSTREAMLOGGER_H

class LevelStreamLogger;

#include "de/hackcraft/log/Logger.h"

#include <sstream>

/**
 * This class is used by the Logger class - don't use directly!
 */
class LevelStreamLogger {
    
public:
    /**
     * Create new level-logger for a logger.
     * 
     * @param logger Logger this sub-logger or level-logger belongs to.
     * @param level The level this sub-logger is used for.
     */
    LevelStreamLogger(Logger* logger, int level) {
        this->logger = logger;
        this->level = level;
    }

    /**
     * Append a value to this level logger 
     * and thus to the logger this level logger belongs to.
     * 
     * @param val Any typed input value that may be used with stringstream.
     */
    template<typename T>
    void append(T const& val) {
        logstream << val;
        if (logger != NULL) {
            appendString(logstream.str().c_str());
            logstream.clear();
            logstream.str("");
        }
    }

private:
    /**
     * Append a string value to this level logger 
     * and thus to the logger this level logger belongs to.
     * 
     * @param val A string value to append to the logger this level logger belongs to.
     */
    void appendString(const char* val);
    
private:
    /** The logger this level logger belongs to. */
    Logger* logger;
    /** The logging level this level logger logs for. */
    int level;
    /** As for now a temporary buffer to input before sending to the logger. */
    std::stringstream logstream;
};


/*
 * This template does the trick of allowing the stream operator
 * on the level logger of a logger:
 * 
 * logger.trace() << "Running operation: " << x << "\n";
 */
template<typename T>
LevelStreamLogger& operator<<(LevelStreamLogger& levelLogger, T const& val) {
    levelLogger.append(val);
    return levelLogger;
}

#endif	/* LEVELSTREAMLOGGER_H */

