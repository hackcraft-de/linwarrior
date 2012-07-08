#include "Logger.h"

Appender* Logger::globalAppender = NULL;


Logger* Logger::getLogger(const char* qualifiedLoggerName) {
    
    Logger* logger = new Logger(qualifiedLoggerName);
    
    return logger;
}


Logger::Logger(const char* qualifiedLoggerName) {
    
    if (globalAppender == NULL) {
        globalAppender = new Appender();
    }
    
    this->qualifiedLoggerName = qualifiedLoggerName;
    this->parentLogger = NULL;
    this->appender = globalAppender;
    
    this->errorLogger = new LevelStreamLogger(this, 0);
    this->warnLogger = new LevelStreamLogger(this, 1);
    this->infoLogger = new LevelStreamLogger(this, 2);
    this->debugLogger = new LevelStreamLogger(this, 3);
    this->traceLogger = new LevelStreamLogger(this, 4);
}


Logger::~Logger() {
    
    delete this->errorLogger;
    delete this->warnLogger;
    delete this->infoLogger;
    delete this->debugLogger;
    delete this->traceLogger;
    
    delete this->appender;
}


LevelStreamLogger& Logger::error() {
    return *errorLogger;
}


LevelStreamLogger& Logger::warn() {
    return *warnLogger;
}


LevelStreamLogger& Logger::info() {
    return *infoLogger;
}


LevelStreamLogger& Logger::debug() {
    return *debugLogger;
}


LevelStreamLogger& Logger::trace() {
    return *traceLogger;
}


void Logger::append(LoggerEvent* loggerEvent) {
    
    // Send event to the appender if there is any.
    if (appender != NULL) {
        appender->append(loggerEvent);
    }

    // Forward event to the parent or stop here and now.
    if (parentLogger != NULL) {
        parentLogger->append(loggerEvent);
    } else {
        delete loggerEvent;
    }
}


const char* Logger::getName() {
    return qualifiedLoggerName.c_str();
}

const char* Logger::getLevelName(int level) {
    
    switch (level) {
        case 0: return "ERROR"; break;
        case 1: return "WARN"; break;
        case 2: return "INFO"; break;
        case 3: return "DEBUG"; break;
        case 4: return "TRACE"; break;
        default: return "UNKNOWN"; break;
    }
}
