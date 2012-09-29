/* 
 * File:    rSoundsource.h
 * Project: LinWarrior 3D
 * Home:    hackcraft.de
 * 
 * Created on August 19, 2012, 12:14 PM
 */


#ifndef RSOUNDSOURCE_H
#define	RSOUNDSOURCE_H

class rSoundsource;

#include "de/hackcraft/psi3d/math3d.h"

#include "de/hackcraft/world/Component.h"

#include <string>

class Entity;

/**
 * A sound-source for playing sound at a given movable position.
 * To be enhanced with volume fading and looping to be useful for
 * variable engine and footstep sounds.
 */
class rSoundsource : public Component {

public: // INPUT
    /** Position of source. */
    vec3 pos0;
    /** Velocity of source (unused). */
    vec3 vel0;
    /** Pitch (default 1) of the sound source. */
    float pitch;
    /** Multiplier (default 1) applied before using pitch. */
    float pitchScaler;
    /** Offset (default 0) added to pitch*pitchScaler. */
    float pitchOffset;
    /** Volume (default 0) of the sound source. */
    float gain;
    /** Multiplier (default 1) applied before using gain. */
    float gainScaler0;
    /** Multiplier (default 1) applied before using gain */
    float gainScaler1;
    /** Offset (default 0) added to gain*gainScaler*gainScaler2. */
    float gainOffset;
    
protected: // INTERNAL
    unsigned int sourceHandle;
    std::string file;
    
public:
    rSoundsource(Entity * obj);
    virtual ~rSoundsource();
    
    void loadWithWav(const char* filename, bool looping);
    void play();
    
    /** Repositions audio source according to set position. */
    virtual void animate(float spf);
};

#endif	/* RSOUNDSOURCE_H */

