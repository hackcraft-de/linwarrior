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

#include <sstream>

class Logger;

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

    /*
    * This template does the trick of allowing the stream operator
    * on the level logger of a logger:
    * 
    * logger.trace() << "Running operation: " << x << "\n";
    */
    template<typename T>
    LevelStreamLogger& operator<<(const T& val) {
        logstream << (int*)(&val);
        appendStream();
        return *this;
    }
    
    /*
    * This template does the trick of allowing the plus operator
    * on the level logger of a logger:
    * 
    * logger.trace() + "Running operation: " + x + "\n";
    */
    template<typename T>
    LevelStreamLogger& operator+(const T& val) {
        logstream << (int*)(&val);
        appendStream();
        return *this;
    }

private:
    /**
     * Called after data was written to the stream of this level logger.
     * Extracts a string (or line) from the stream and sends to the logger
     * to which this level logger belongs to.
     */
    void appendStream();
    
private:
    /** The logger this level logger belongs to. */
    Logger* logger;
    /** The logging level this level logger logs for. */
    int level;
    /** As for now a temporary buffer to input before sending to the logger. */
    std::stringstream logstream;
};


#endif	/* LEVELSTREAMLOGGER_H */

