/* 
 * File:     Thread.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on September 24, 2011, 10:07 PM
 */

#ifndef THREAD_H
#define	THREAD_H

#include "de/hackcraft/lang/Object.h"
#include "de/hackcraft/lang/Runnable.h"

/**
 * Thread class for starting and maintaining runnable threads.
 */
class Thread : public Object, public Runnable {
private:
    Runnable* runnable;
    void* threadhandle;
public:
    Thread();
    Thread(Runnable *runnable);
    void start();
    void stop();
    void sleep(unsigned int ms);
    virtual void run() = 0;
    virtual int hashCode();
};


#endif	/* THREAD_H */

