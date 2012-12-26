#include "Semaphore.h"

#include <SDL/SDL_thread.h>


Semaphore::Semaphore(int permits) {
    
    SDL_sem* sem = SDL_CreateSemaphore((Uint32) permits);
    semaphore = (void*) sem;
}


Semaphore::~Semaphore() {

    SDL_sem* sem = (SDL_sem*) semaphore;
    SDL_DestroySemaphore(sem);
}


bool Semaphore::tryAcquire() {
    
    SDL_sem* sem = (SDL_sem*) semaphore;
    int err = SDL_SemTryWait(sem);

    if (err == 0) {
        return true;
    } else if (err == -1) {
        throw "Error while acquiring semaphore lock.";
    } else {
        return false;
    }
}


void Semaphore::acquire() {
    
    SDL_sem* sem = (SDL_sem*) semaphore;
    int err = SDL_SemWait(sem);
    
    if (err != 0) {
        throw "Error while acquiring semaphore lock.";
    }
}


void Semaphore::release() {
    
    SDL_sem* sem = (SDL_sem*) semaphore;
    int err = SDL_SemPost(sem);
    
    if (err != 0) {
        throw "Error while releasing semaphore lock.";
    }
}

