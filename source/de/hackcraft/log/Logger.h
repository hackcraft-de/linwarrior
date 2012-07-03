/* 
 * File:    Logger.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on July 3, 2012, 6:58 PM
 */


#ifndef LOGGER_H
#define	LOGGER_H

class Logger;

#include "de/hackcraft/log/Appender.h"
#include "de/hackcraft/log/LevelStreamLogger.h"

#include <string>

/**
 * Use this class to create and use Loggers.
 * 
 * 1. Create a static logger instance:
 * 
 *      static Logger* logger;
 *      ...
 *      logger = Logger::getLogger("de.hackcraft.game.Myclass");
 * 
 * 2. Use to log:
 * 
 *      logger->trace() %lt;%lt; "This is trace level message no. " %lt;%lt; 1 %lt;%lt; "\n";
 * 
 * Note: Hierarchical logging isn't implemented as for now.
 */
class Logger {

public:
    /** Get already available logger instance or get and register new instance. */
    static Logger* getLogger(const char* qualifiedLoggerName);
    
    /** Tear down logger and free resources. */
    virtual ~Logger();

public:
    /** Get writable input stream for: error level */
    LevelStreamLogger& error();
    
    /** Get writable input stream for: warn level */
    LevelStreamLogger& warn();
    
    /** Get writable input stream for: info level */
    LevelStreamLogger& info();
    
    /** Get writable input stream for: debug level */
    LevelStreamLogger& debug();
    
    /** Get writable input stream for: trace level */
    LevelStreamLogger& trace();

    /** Write string to logger with given level */
    void append(int level, const char* loggerOutput);
    
private:
    /** Use getLogger instead to get the benefit of shared loggers (future). */
    Logger(const char* qualifiedLoggerName);

private:
    /** Name of logger with full class path. */
    std::string qualifiedLoggerName;
    
    /** Future: Parent Logger may actually be grand grand.. parent if there is no in-between. */
    Logger* parentLogger;

    /** Logger input is forwarded to this appender (if not null) which will output the input. */
    Appender* appender;

    // Individual sub-loggers for providing logger input for different levels.
    LevelStreamLogger* errorLogger;
    LevelStreamLogger* warnLogger;
    LevelStreamLogger* infoLogger;
    LevelStreamLogger* debugLogger;
    LevelStreamLogger* traceLogger;
};


#endif	/* LOGGER_H */
