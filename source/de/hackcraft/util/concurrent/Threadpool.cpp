#include "Threadpool.h"

#include "de/hackcraft/lang/Runnable.h"

#include "de/hackcraft/util/concurrent/Semaphore.h"
#include "de/hackcraft/util/concurrent/Minion.h"

#include <stdlib.h>


Threadpool::Threadpool() {
    
    jobMutex = new Semaphore(1);
    jobQueue = new std::queue<Runnable*>();
    minions = new std::vector<Minion*>;
}


Threadpool::~Threadpool() {

    shutdown();
    
    delete minions;
    delete jobMutex;
    delete jobQueue;            
}


void Threadpool::addJob(Runnable* job) {
    
    jobQueue->push(job);
}


void Threadpool::addThreads(int n) {
    
    for (int i = 0; i < n; i++) {
        
        Minion* minion = new Minion(this->jobMutex, this->jobQueue);
        minion->start();
        minions->push_back(minion);
    }
}


void Threadpool::shutdown() {

    if (minions == NULL) {
        return;
    }
    
    while (!minions->empty()) {
        
        Minion* minion = minions->back();
        minions->pop_back();
        
        minion->stop();
        delete minion;
    }
}

