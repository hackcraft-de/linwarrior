#include "rSoundsource.h"

#include "de/hackcraft/io/Filesystem.h"

#include "de/hackcraft/openal/AL.h"


rSoundsource::rSoundsource(Entity * obj) {
    
    this->object = obj;
    this->sourceHandle = 0;
    
    vector_zero(pos0);
    vector_zero(vel0);
    
    pitch = 1;
    pitchScaler = 1;
    pitchOffset = 0;
    
    gain = 1;
    gainScaler0 = 1;
    gainScaler1 = 1;
    gainOffset = 0;
}


rSoundsource::~rSoundsource() {
}


void rSoundsource::loadWithWav(const char* filename, bool looping) {
    
    try {
        ALuint buffer;
        
        if (filename == NULL) {
                buffer = alutCreateBufferHelloWorld();
        } else {
                long fileImageSize = 0;
                char* fileImage = Filesystem::loadBinaryFile(filename, &fileImageSize);
                buffer = alutCreateBufferFromFileImage(fileImage, fileImageSize);
        }
        
        if (buffer == AL_NONE) throw "could not load buffer";
        
        ALuint* soundsource = &sourceHandle;
        alGenSources(1, soundsource);
        
        if (alGetError() == AL_NO_ERROR && alIsSource(*soundsource)) {
            alSourcei(*soundsource, AL_BUFFER, buffer);
            alSourcef(*soundsource, AL_PITCH, pitch*pitchScaler + pitchOffset);
            alSourcef(*soundsource, AL_GAIN, gain*gainScaler0*gainScaler1 + gainOffset);
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
        
        alSourcef(sourceHandle, AL_PITCH, fabs(pitch)*pitchScaler + pitchOffset);
        alSourcef(sourceHandle, AL_GAIN, fabs(gain)*gainScaler0*gainScaler1 + gainOffset);
    }
}

