/* 
 * File:     Minion.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on December 26, 2012, 2:19 AM
 */

#ifndef MINION_H
#define	MINION_H

class Minion;

#include "de/hackcraft/lang/Thread.h"

#include <queue>

class Logger;
class Runnable;

struct SDL_mutex;

/**
 * Models a minion worker thread that fetches jobs
 * from the main job queue.
 */
class Minion : public Thread {
private:
    static Logger* logger;
    /** Pointing to the main job-mutex. To be replaced with framework class. */
    SDL_mutex* jobMutex;
    /** Pointing to the main job-queue. */
    std::queue<Runnable*>* jobQueue;
public:
    Minion(SDL_mutex* jobMutex, std::queue<Runnable*>* jobQueue);
    void run();
};

#endif	/* MINION_H */

