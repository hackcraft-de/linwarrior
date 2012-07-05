#include "LoggerEvent.h"


LoggerEvent::LoggerEvent(Logger* logger, int level, const char* message, unsigned int tid) {
    this->logger = logger;
    this->level = level;
    this->message = message;
    this->tid = tid;
}
 

LoggerEvent::~LoggerEvent() {
}


Logger* LoggerEvent::getLogger() {
    return logger;
}


int LoggerEvent::getLevel() {
    return level;
}


const char* LoggerEvent::getMessage() {
    return message.c_str();
}

unsigned int LoggerEvent::getTid() {
    return tid;
}

