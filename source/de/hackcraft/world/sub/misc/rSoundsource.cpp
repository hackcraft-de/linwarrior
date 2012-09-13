#include "rSoundsource.h"

#include "de/hackcraft/openal/AL.h"


rSoundsource::rSoundsource(Entity * obj) {
    
    this->object = obj;
    this->sourceHandle = 0;
    
    vector_zero(pos0);
    vector_zero(vel0);
}


rSoundsource::~rSoundsource() {
}


void rSoundsource::loadWithWav(const char* filename) {
    
    try {
        ALuint buffer;
        //buffer = alutCreateBufferHelloWorld();
        buffer = alutCreateBufferFromFile("data/base/device/pow.wav");
        //if (buffer == AL_NONE) throw "could not load pow.wav";
        ALuint* soundsource = &sourceHandle;
        alGenSources(1, soundsource);
        if (alGetError() == AL_NO_ERROR && alIsSource(*soundsource)) {
            alSourcei(*soundsource, AL_BUFFER, buffer);
            alSourcef(*soundsource, AL_PITCH, 1.0f);
            alSourcef(*soundsource, AL_GAIN, 1.0f);
            alSourcefv(*soundsource, AL_POSITION, pos0);
            alSourcefv(*soundsource, AL_VELOCITY, vel0);
            alSourcei(*soundsource, AL_LOOPING, AL_FALSE);
        }
    } catch (...) {
        throw "loadWithWav: Could not create sound source.\n";
    }
}


void rSoundsource::play() {
    
    bool validSource = alIsSource(sourceHandle);
    
    if (validSource) {
        alSourcePlay(sourceHandle);
    }
}


void rSoundsource::animate(float spf) {
    
    bool validSource = alIsSource(sourceHandle);
    
    if (validSource) {        
        alSourcefv(sourceHandle, AL_POSITION, pos0);
        //alSourcefv(sourceHandle, AL_VELOCITY, vel0);
    }
}

