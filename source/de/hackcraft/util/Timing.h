/* 
 * File:     cTiming.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on 23. Dezember 2010, 14:54
 */

#ifndef CTIMING_H
#define	CTIMING_H

class Timing;

#include "de/hackcraft/lang/OID.h"

#include <string>


/**
 * Class that encapsulates the simulation time and animation timing.
 * 
 * Serious question: What time & date format would be used
 * as a common planetary calendar? Is that sensible at all?!
 * So far I assume terran planetary dates everywhere.
 * But time & date should be adjusted to the planet and
 * planet's date shall be mapped like beween maya, gregorian
 * or moon calendars.
 */
class Timing {
private:
    /** Counts years of simulation universe. */
    unsigned int mYear;

    /** Counts 366-Days of simulation year (0-365). */
    unsigned int mDay;

    /** Counts 24-Hours of simulation day (0-23). */
    unsigned int mHour;

    /** Counts minutes of simulation hour. */
    unsigned int mMinute;

    /** Counts seconds of simulation minute. */
    unsigned int mSecond;

    /** Micro-Seconds, incremented by delta at every simulation timestep. */
    unsigned int mMSec;

    /** Incremented at every simulation timestep and reset on each second. */
    unsigned int mFrame;

    /** Incremented for noteable events (eg. OID consumption). Reset each frame. */
    unsigned int mDeltacycle;

    /** seconds per frame = 1.0f/mFPS. */
    float mSPF;

    /** frames per second = 1.0f/mSPF. */
    float mFPS;
public:

    Timing();
    //cTiming(const cTiming& orig);
    //virtual ~cTiming();

    /** The seconds per frame or frame-rate-inverse. */
    float getSPF();
    
    /** The current deltacycle (i-th event within a frame). */
    unsigned int getDeltacycle();
    
    /** The current frame number. */
    unsigned int getFrame();

    /** Set current frames per seconds and derived seconds per frame. */
    void setFPS(float fps);
    
    /** Set year and day in year. */
    void setDate(unsigned int year, unsigned int day);
    
    /** Set hour, minute and second of day. */
    void setTime(unsigned hour, unsigned minute = 0, unsigned second = 0);

    /** Increment deltacycle by one (within-frame-notable-event-counter). */
    void advanceDelta();
    
    /** Increment time by the amount of milliseconds and propagate datetime changes. */
    void advanceTime(int deltamsec);

    /** The time in 24-hours format as a float - ie. 0.00 to 23.99 (vs. 23.59) used for interpolation. */
    float getTime24();
    
    /** Derive a unique key from current time (using deltacycle which needs to be incremented afterwards). */
    OID getTimekey();
    
    /** Get date-string YYYY-MM-DD. */
    std::string getDate();
    
    /** Get time-string HH:MM:SS. */
    std::string getTime();

};


#endif	/* CTIMING_H */

