#include "LevelStreamLogger.h"

void LevelStreamLogger::appendString(const char* val) {
    logger->append(level, logstream.str().c_str());
}
