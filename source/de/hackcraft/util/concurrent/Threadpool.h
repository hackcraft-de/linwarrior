/* 
 * File:     Threadpool.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on March 3, 2013, 11:50 AM
 */

#ifndef THREADPOOL_H
#define	THREADPOOL_H

class Threadpool;

#include <vector>
#include <queue>

class Minion;
class Runnable;
class Semaphore;

/**
 * Container for running worker threads (minions) that work on runnable tasks.
 */
class Threadpool {
public:
    Threadpool();
    virtual ~Threadpool();
    
    void addJob(Runnable* job);
    
    void addThreads(int n);
    void shutdown();
private:

    /** Mutex for job queue - minions wait until they can grab a job. */
    Semaphore* jobMutex;

    /** The job queue itself - minions grab jobs here. */
    std::queue<Runnable*>* jobQueue;
    
    std::vector<Minion*>* minions;
};

#endif	/* THREADPOOL_H */

