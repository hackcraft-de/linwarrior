#include "Minion.h"

#include "de/hackcraft/lang/Runnable.h"

#include "de/hackcraft/log/Logger.h"

#include "de/hackcraft/util/concurrent/Semaphore.h"


Logger* Minion::logger = Logger::getLogger("de.hackcraft.game.Minion");


Minion::Minion(Semaphore* jobMutex, std::queue<Runnable*>* jobQueue) {
    this->jobMutex = jobMutex;
    this->jobQueue = jobQueue;
}


void Minion::run() {
    unsigned long id = (unsigned long) this;//SDL_ThreadID();
    logger->info() << "Minion " << id << " at your service!\n";
    Runnable* job = (Runnable*) this;
    bool done = false;

    while (!done) {
        // Grab a new job.
        Runnable* nextjob = NULL;
        jobMutex->acquire();
        {
            if (!jobQueue->empty()) {
                nextjob = jobQueue->front();
                jobQueue->pop();
            } // else look at secondary jobs.
        }
        jobMutex->release();
        // Work on job if any available.
        if (nextjob != NULL) {
            logger->debug() << "Minion " << id << " is fulfilling your wish!\n";
            job = nextjob;
            job->run();
            int result = 0;
            logger->debug() << "Minion " << id << " job is finished with result " << result << "!\n";
        } else if (job != NULL && nextjob == NULL) {
            // Wait for a job to do.
            logger->debug() << "Minion " << id << " is awaiting your command!\n";
            job = nextjob;
            Thread::sleep(10);
        } else {
            Thread::sleep(10);
        }
    }
}
