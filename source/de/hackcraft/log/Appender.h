/* 
 * File:    Appender.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on July 3, 2012, 7:06 PM
 */


#ifndef APPENDER_H
#define	APPENDER_H

class Appender;

#include "de/hackcraft/log/LoggerEvent.h"

/**
 * Used by the Logger class to output logger input.
 * This Appender class currently just forwards to std::cout.
 * Subclass and override append-method to output elsewhere
 * or to change the output format.
 */
class Appender {
public:
    Appender();
    virtual ~Appender();
    
    virtual void append(LoggerEvent* loggerEvent);
    
    void setMaxLevel(int maxLevel);
    int getMaxLevel();
    
private:
    /** Maximum level that's still appended to output (printed). */
    int maxLevel;
};

#endif	/* APPENDER_H */

