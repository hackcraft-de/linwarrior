#include "rSoundsource.h"

#include "de/hackcraft/openal/AL.h"


rSoundsource::rSoundsource(Entity * obj) {
    
    this->object = obj;
    this->sourceHandle = 0;
    
    vector_zero(pos0);
    vector_zero(vel0);
    
    pitch = 1;
    gain = 1;
}


rSoundsource::~rSoundsource() {
}


void rSoundsource::loadWithWav(const char* filename, bool looping) {
    
    try {
        ALuint buffer;
        
        if (filename == NULL) {
                buffer = alutCreateBufferHelloWorld();
        } else {
                buffer = alutCreateBufferFromFile(filename);
        }
        
        if (buffer == AL_NONE) throw "could not load buffer";
        
        ALuint* soundsource = &sourceHandle;
        alGenSources(1, soundsource);
        
        if (alGetError() == AL_NO_ERROR && alIsSource(*soundsource)) {
            alSourcei(*soundsource, AL_BUFFER, buffer);
            alSourcef(*soundsource, AL_PITCH, pitch);
            alSourcef(*soundsource, AL_GAIN, gain);
            alSourcefv(*soundsource, AL_POSITION, pos0);
            alSourcefv(*soundsource, AL_VELOCITY, vel0);
            alSourcei(*soundsource, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
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
        
        alSourcef(sourceHandle, AL_PITCH, pitch);
        alSourcef(sourceHandle, AL_GAIN, gain);
    }
}

