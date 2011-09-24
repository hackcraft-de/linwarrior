/* 
 * File:     Runnable.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on September 24, 2011, 11:01 PM
 */

#ifndef RUNNABLE_H
#define	RUNNABLE_H


/**
 * Thread-runnable object interface.
 */
class Runnable {
public:
    virtual void run() = 0;
};

#endif	/* RUNNABLE_H */

