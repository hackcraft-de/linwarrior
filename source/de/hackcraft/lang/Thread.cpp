#include "Thread.h"

#include <SDL/SDL_thread.h>
#include <SDL/SDL_timer.h>


// Just serves to start the Runnable run method after a thread fork.
int runRunnable(void* data) {
    Runnable* r = (Runnable*) data;
    r->run();
    return 0;
}


Thread::Thread() {
    this->runnable = this;
    this->threadhandle = NULL;
}


Thread::Thread(Runnable *runnable) {
    this->runnable = runnable;
    this->threadhandle = NULL;
}


void Thread::start() {
    threadhandle = (void*) SDL_CreateThread(runRunnable, runnable);
}


void Thread::stop() {
    SDL_KillThread((SDL_Thread*) threadhandle);
}


void Thread::sleep(unsigned int ms) {
    SDL_Delay(ms);
}


int Thread::hashCode() {
    return (int)((unsigned long long)this & 0xFFFFFFFF);
}

