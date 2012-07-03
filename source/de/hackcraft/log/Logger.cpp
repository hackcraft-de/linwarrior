#include "Logger.h"


Logger* Logger::getLogger(const char* qualifiedLoggerName) {
    
    Logger* logger = new Logger(qualifiedLoggerName);
    
    return logger;
}


Logger::Logger(const char* qualifiedLoggerName) {
    
    this->qualifiedLoggerName = qualifiedLoggerName;
    this->parentLogger = NULL;
    this->appender = new Appender();
    
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


void Logger::append(int level, const char* loggerOutput) {
    appender->append(loggerOutput);
}
