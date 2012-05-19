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

#include "de/hackcraft/world/OID.h"

#include <string>


// Serious question: What time & date format would be used
// as a common planetary calendar? Is that sensible at all?!
// So far I assume terran planetary dates everywhere.
// But time & date should be adjusted to the planet and
// planet's date shall be mapped like beween maya, gregorian
// or moon calendars.

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

    float getSPF();
    unsigned int getDeltacycle();
    unsigned int getFrame();

    void setFPS(float fps);
    void setDate(unsigned int year, unsigned int day);
    void setTime(unsigned hour, unsigned minute = 0, unsigned second = 0);

    void advanceDelta();
    void advanceTime(int deltamsec);

    float getTime24();
    OID getTimekey();
    std::string getDate();
    std::string getTime();

};


#endif	/* CTIMING_H */

