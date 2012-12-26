/* 
 * File:     Semaphore.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on December 26, 2012, 7:50 AM
 */

#ifndef SEMAPHORE_H
#define	SEMAPHORE_H

#include "de/hackcraft/lang/Object.h"

class Semaphore : public Object {
public:
    /** Create a semaphore with max amount of permits (1 permit => mutex). */
    Semaphore(int permits);
    virtual ~Semaphore();
    
    /** Try to get one permit when available and then acquire the permit else return false. */
    bool tryAcquire();
    /** Block until one permit is available and then acquire the permit. */
    void acquire();
    /** Release one previously acquired permit. */
    void release();
private:
    /** Handle for actual low level implementation. */
    void* semaphore;
};

#endif	/* SEMAPHORE_H */

