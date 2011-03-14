/* 
 * File:     cTiming.cpp
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 * 
 * Created on 23. Dezember 2010, 14:54
 */

#include "cTiming.h"

#include <iostream>
#include <sstream>
#include <iomanip>


cTiming::cTiming() {
    setDate(0, 0);
    setTime(0, 0, 0);
    setFPS(30);
}

/*
cTiming::cTiming(const cTiming& orig) {
}

cTiming::~cTiming() {
}
*/

float cTiming::getSPF() {
    return mSPF;
}

unsigned int cTiming::getDeltacycle() {
    return mDeltacycle;
}

unsigned int cTiming::getFrame() {
    return mFrame;
}

void cTiming::setFPS(float fps) {
    mFPS = fps;
    mSPF = 1.0f / fps;
}

void cTiming::setDate(unsigned int year, unsigned int day) {
    mYear = year;
    mDay = day;
}

void cTiming::setTime(unsigned hour, unsigned minute, unsigned second) {
    mHour = hour;
    mMinute = minute;
    mSecond = second;
    mMSec = 0;
    mFrame = 0;
    mDeltacycle = 1;
}

void cTiming::advanceDelta() {
    mDeltacycle++;
}

void cTiming::advanceTime(int deltamsec) {
    mSPF = float(deltamsec) / 1000.0f;
    mFPS = 1.0f / (mSPF + 0.000001f);
    mFrame++;
    mDeltacycle = 1;
    mMSec += deltamsec;
    if (mMSec >= 1000) {
        mFrame = 0;
        mSecond += mMSec / 1000;
        mMSec %= 1000;
        if (mSecond >= 60) {
            mMinute += mSecond / 60;
            mSecond %= 60;
            if (mMinute >= 60) {
                mHour += mMinute / 60;
                mMinute %= 60;
                if (mHour >= 24) {
                    mDay += mHour / 24;
                    mHour %= 24;
                    if (mDay >= 366) {
                        mYear += mDay / 366;
                        mDay %= 366;
                    }
                }
            }
        }
    }
}

float cTiming::getTime24() {
    float time24 = float(mHour) + float(mMinute) / 60.0f + float(mSecond) / (60.0f * 60.0f) + float(mMSec) / (60.0f * 60.0f * 1000.0f);
    return time24;
}

OID cTiming::getTimekey() {
    OID ddd = mDay;
    OID hh = mHour;
    OID mm = mMinute;
    OID ss = mSecond;
    OID ff = mFrame;
    OID serid = ((((ddd * 100 + hh) *100 + mm) *100 + ss) * 100 + ff) * 10000 + mDeltacycle;
    return serid;
}

std::string cTiming::getDate() {
    std::stringstream s;
    s << mYear << "_" << mDay;
    return s.str();
}

std::string cTiming::getTime() {
    std::stringstream s;
    s << mHour;
    s << ":" << std::setfill('0') << std::setw(2) << mMinute;
    s << ":" << std::setfill('0') << std::setw(2) << mSecond;
    return s.str();
}
